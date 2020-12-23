#include "hybridgeqt.h"
#include "qtmeta.h"
#include "variant.h"

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QUuid>

HybridgeQt::HybridgeQt()
{
    timer_.bridge = this;
    QObject::connect(&timer_, &QTimer::timeout, [this] () {
        timerEvent();
    });
}

MetaObject *HybridgeQt::metaObject(const Object *object) const
{
    QtMetaObject * m = objectMetas_.value(object);
    if (m == nullptr) {
        m = new QtMetaObject(*static_cast<QObject const *>(object)->metaObject());
        objectMetas_[object] = m;
    }
    return m;
}

std::string HybridgeQt::createUuid() const
{
    return QUuid::createUuid().toString().toUtf8().data();
}

MetaObject::Connection HybridgeQt::connect(const Object *object, int signalIndex)
{
    static const int memberOffset = QTimer::staticMetaObject.methodCount();
    QMetaObject::connect(static_cast<QObject const *>(object), signalIndex,
                         &timer_, memberOffset + signalIndex, Qt::AutoConnection, 0);
    //QObject::connect(object, signalIndex, object, [] )
    return MetaObject::Connection(object, signalIndex);
}

bool HybridgeQt::disconnect(const MetaObject::Connection &c)
{
    QObject const * obj = static_cast<QObject const *>(c.object());
    return QObject::disconnect(obj, obj->metaObject()->method(c.signalIndex()),
                        static_cast<QObject*>(nullptr), QMetaMethod());
}

void HybridgeQt::startTimer(int msec)
{
    timer_.start(msec);
}

void HybridgeQt::stopTimer()
{
    timer_.stop();
}

void HybridgeQt::dispatch(const QObject *object, const int signalIdx, void **argumentData)
{
    QMetaMethod method = object->metaObject()->method(signalIdx);
    Array arguments;
    arguments.reserve(static_cast<size_t>(method.parameterCount()));
    // TODO: basic overload resolution based on number of arguments?
    for (int i = 0; i < method.parameterCount(); ++i) {
        const QMetaType::Type type = static_cast<QMetaType::Type>(method.parameterType(i));
        QVariant arg;
        if (type == QMetaType::QVariant) {
            arg = *reinterpret_cast<QVariant *>(argumentData[i + 1]);
        } else {
            arg = QVariant(type, argumentData[i + 1]);
        }
        arguments.emplace_back(Variant::toValue(arg));
    }
    signal(object, signalIdx, std::move(arguments));
}

int HybridgeQt::Timer::qt_metacall(QMetaObject::Call call, int methodId, void **args)
{
    methodId = QObject::qt_metacall(call, methodId, args);
    if (methodId < 0)
        return methodId;

    if (call == QMetaObject::InvokeMetaMethod) {
        const QObject *object = sender();
        Q_ASSERT(object);
        Q_ASSERT(senderSignalIndex() == methodId);
        //Q_ASSERT(m_connectionsCounter.contains(object));
        //Q_ASSERT(m_connectionsCounter.value(object).contains(methodId));

        bridge->dispatch(object, methodId, args);

        return -1;
    }
    return methodId;
}
