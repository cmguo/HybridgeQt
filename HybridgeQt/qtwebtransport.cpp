#include "qtwebtransport.h"
#include "qtvariant.h"

#include <QJsonObject>
#include <QWebChannelAbstractTransport>

// new window.QWebChannel(qt.webChannelTransport,(function(e){
//
//})

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

void QtWebTransport::sendMessage(Message &&message)
{
    QVariant v = QtVariant::fromValue(Value(const_cast<Message&>(message)));
    transport_->sendMessage(QJsonObject::fromVariantMap(v.toMap()));
}

void QtWebTransport::messageReceived(const QJsonObject &message, QWebChannelAbstractTransport *transport)
{
    assert(transport == transport_);
    QVariant v(message);
    Map emptyMap;
    Transport::messageReceived(std::move(QtVariant::toValue(v).toMap(emptyMap)));
}
