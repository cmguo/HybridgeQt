#ifndef QTCHANNEL_H
#define QTCHANNEL_H

#include "HybridgeQt_global.h"

#include <core/channel.h>

#include <QMap>
#include <QTimer>

class QtMetaObject;

class HYBRIDGEQT_EXPORT QtChannel : public Channel
{
public:
    QtChannel();

public:
    void connectTo(Transport *transport, std::function<void(QMap<QByteArray, QObject*>)> receive = nullptr);

    // Bridge interface
protected:
    virtual MetaObject *metaObject(const Object *object) const override;
    virtual std::string createUuid() const override;
    virtual ProxyObject * createProxyObject(Map &&meta) const override;
    virtual void startTimer(int msec) override;
    virtual void stopTimer() override;

private:
    friend class SignalReceiver;

    QTimer timer_;
    mutable QMap<Object const *, QtMetaObject*> objectMetas_;
};

#endif // QTCHANNEL_H
