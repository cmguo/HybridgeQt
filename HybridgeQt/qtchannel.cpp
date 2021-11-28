#include "qtchannel.h"
#include "qtmeta.h"
#include "qtproxyobject.h"
#include "qtvariant.h"

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QUuid>

QtChannel::QtChannel()
{
    QMetaType::registerConverter<QVariantMap, QMap<QByteArray, QObject*>>([] (auto & map) {
        QMap<QByteArray, QObject*> result;
        auto it = map.begin();
        for (; it != map.end(); ++it) {
            result.insert(it.key().toUtf8(), it.value().template value<QObject*>());
        }
        return result;
    });
    QObject::connect(&timer_, &QTimer::timeout, [this] () {
        timerEvent();
    });
}

void QtChannel::connectTo(Transport *transport, std::function<void (QMap<QByteArray, QObject*>)> receive)
{
    MetaMethod::Response receive2 = receive ? [receive](Value && result) {
        qDebug() << QtVariant::fromValue(result);
        receive(QtVariant::fromValue(result).value<QMap<QByteArray, QObject*>>());
    } : MetaMethod::Response(nullptr);
    return Channel::connectTo(transport, receive2);
}

MetaObject *QtChannel::metaObject(const Object *object) const
{
    QtMetaObject * m = objectMetas_.value(object);
    if (m == nullptr) {
        m = new QtMetaObject(*static_cast<QObject const *>(object)->metaObject());
        objectMetas_[object] = m;
    }
    return m;
}

std::string QtChannel::createUuid() const
{
    return QUuid::createUuid().toString().toUtf8().data();
}

ProxyObject *QtChannel::createProxyObject(Map &&meta) const
{
    return new QtProxyObject(std::move(meta));
}

void QtChannel::startTimer(int msec)
{
    timer_.start(msec);
}

void QtChannel::stopTimer()
{
    timer_.stop();
}

