#ifndef HYBRIDGEQT_H
#define HYBRIDGEQT_H

#include "HybridgeQt_global.h"

#include <core/bridge.h>

#include <QMap>
#include <QTimer>

class QtMetaObject;

class HYBRIDGEQT_EXPORT HybridgeQt : Bridge
{
public:
    HybridgeQt();

    // Bridge interface
protected:
    virtual MetaObject *metaObject(const Object *object) const override;
    virtual std::string createUuid() const override;
    virtual MetaObject::Connection connect(const Object *object, int signalIndex) override;
    virtual bool disconnect(const MetaObject::Connection &c) override;
    virtual void startTimer(int msec) override;
    virtual void stopTimer() override;

private:
    friend class Timer;
    void dispatch(const QObject *object, const int signalIdx, void **argumentData);

private:
    class Timer : public QTimer
    {
    public:
        int qt_metacall(QMetaObject::Call call, int methodId, void **args) override;
        HybridgeQt * bridge;
    };
    Timer timer_;
    mutable QMap<Object const *, QtMetaObject*> objectMetas_;
};

#endif // HYBRIDGEQT_H
