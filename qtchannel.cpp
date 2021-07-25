#include "qtchannel.h"
#include "qtmeta.h"
#include "qtvariant.h"

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QUuid>

QtChannel::QtChannel()
{
    QObject::connect(&timer_, &QTimer::timeout, [this] () {
        timerEvent();
    });
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

void QtChannel::startTimer(int msec)
{
    timer_.start(msec);
}

void QtChannel::stopTimer()
{
    timer_.stop();
}

