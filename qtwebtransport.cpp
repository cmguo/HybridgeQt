#include "qtwebtransport.h"
#include "variant.h"

#include <QJsonObject>
#include <QWebChannelAbstractTransport>

QtWebTransport::QtWebTransport(QWebChannelAbstractTransport * transport)
    : transport_(transport)
{
    QObject::connect(transport, &QWebChannelAbstractTransport::messageReceived, [this] (
                     const QJsonObject &message, QWebChannelAbstractTransport *transport) {
        messageReceived(message, transport);
    });
    QObject::connect(transport, &QObject::destroyed, [this] () {
        delete this;
    });
}

void QtWebTransport::sendMessage(const Message &message)
{
    QVariant v = Variant::fromValue(Value::ref(const_cast<Message&>(message)));
    transport_->sendMessage(QJsonObject::fromVariantMap(v.toMap()));
}

void QtWebTransport::messageReceived(const QJsonObject &message, QWebChannelAbstractTransport *transport)
{
    assert(transport == transport_);
    QVariant v(message);
    Map emptyMap;
    Transport::messageReceived(std::move(Variant::toValue(v).toMap(emptyMap)));
}
