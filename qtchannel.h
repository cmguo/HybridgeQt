#ifndef QTCHANNEL_H
#define QTCHANNEL_H

#include "HybridgeQt_global.h"

#include <core/channel.h>

#include <QMap>
#include <QTimer>

class QtMetaObject;

class HYBRIDGEQT_EXPORT QtChannel : Channel
{
public:
    QtChannel();

    // Bridge interface
protected:
    virtual MetaObject *metaObject(const Object *object) const override;
    virtual std::string createUuid() const override;
    virtual void startTimer(int msec) override;
    virtual void stopTimer() override;

private:
    friend class SignalReceiver;

    QTimer timer_;
    mutable QMap<Object const *, QtMetaObject*> objectMetas_;
};

#endif // QTCHANNEL_H
