#include "qtmeta.h"
#include "qtvariant.h"
#include "qtchannel.h"

#include <QMetaObject>
#include <QMetaProperty>

/* QtMetaObject */

QtMetaObject::QtMetaObject(const QMetaObject &meta)
    : meta_(meta)
{
    for (int i = 0; i < meta_.enumeratorCount(); ++i) {
        metaEnums_.push_back(QtMetaEnum(meta_.enumerator(i)));
    }
    for (int i = 0; i < meta_.methodCount(); ++i) {
        metaMethods_.push_back(QtMetaMethod(meta_.method(i)));
    }
    for (int i = 0; i < meta_.propertyCount(); ++i) {
        metaProps_.push_back(QtMetaProperty(meta_.property(i), metaMethods_));
    }
}

const char *QtMetaObject::className() const
{
    return meta_.className();
}

size_t QtMetaObject::propertyCount() const
{
    return metaProps_.size();
}

const MetaProperty &QtMetaObject::property(size_t index) const
{
    return metaProps_.at(index);
}

size_t QtMetaObject::methodCount() const
{
    return static_cast<size_t>(metaMethods_.size());
}

const MetaMethod &QtMetaObject::method(size_t index) const
{
    return metaMethods_.at(index);
}

size_t QtMetaObject::enumeratorCount() const
{
    return metaEnums_.size();
}

const MetaEnum &QtMetaObject::enumerator(size_t index) const
{
    return metaEnums_.at(index);
}

bool SignalReceiver::connect(const MetaObject::Connection &c) const
{
    static const int memberOffset = staticMetaObject.methodCount();
    QObject const * obj = static_cast<QObject const *>(c.object());
    QMetaObject::connect(static_cast<QObject const *>(obj), static_cast<int>(c.signalIndex()),
                         this, memberOffset + static_cast<int>(c.signalIndex()), Qt::AutoConnection, nullptr);
    return true;
}

bool SignalReceiver::disconnect(const MetaObject::Connection &c) const
{
    QObject const * obj = static_cast<QObject const *>(c.object());
    return QObject::disconnect(obj, obj->metaObject()->method(static_cast<int>(c.signalIndex())),
                        this, QMetaMethod());
}

void SignalReceiver::dispatch(const QObject *object, const int signalIdx, void **argumentData)
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
        arguments.emplace_back(QtVariant::toValue(arg));
    }
    auto it = std::find(connections_.begin(), connections_.end(),
              MetaObject::Connection(object, static_cast<size_t>(signalIdx)));
    if (it != connections_.end())
        it->signal(std::move(arguments));
}

int SignalReceiver::qt_metacall(QMetaObject::Call call, int methodId, void **args)
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

        dispatch(object, methodId, args);

        return -1;
    }
    return methodId;
}

static SignalReceiver & signalReceiver(void * receiver)
{
    static std::map<void *, SignalReceiver*> receivers;
    auto it = receivers.find(receiver);
    if (it == receivers.end()) {
        it = receivers.emplace(receiver, new SignalReceiver).first;
    }
    return *it->second;
}

bool QtMetaObject::connect(const Connection &c) const
{
    return signalReceiver(c.receiver()).connect(c);
}

bool QtMetaObject::disconnect(const Connection &c) const
{
    return signalReceiver(c.receiver()).disconnect(c);
}

static QtMetaMethod emptyMethod = QMetaMethod();

QtMetaProperty::QtMetaProperty(const QMetaProperty &meta, std::vector<QtMetaMethod> const & methods)
    : meta_(meta)
    , signal_(meta.hasNotifySignal()
              ? methods.at(static_cast<size_t>(meta.notifySignalIndex())) : emptyMethod)
{
}

const char *QtMetaProperty::name() const
{
    return meta_.name();
}

bool QtMetaProperty::isValid() const
{
    return meta_.isValid();
}

Value::Type QtMetaProperty::type() const
{
    return QtVariant::type(meta_.userType());
}

bool QtMetaProperty::isConstant() const
{
    return meta_.isConstant();
}

size_t QtMetaProperty::propertyIndex() const
{
    return static_cast<size_t>(meta_.propertyIndex());
}

bool QtMetaProperty::hasNotifySignal() const
{
    return meta_.hasNotifySignal();
}

size_t QtMetaProperty::notifySignalIndex() const
{
    return static_cast<size_t>(meta_.notifySignalIndex());
}

const MetaMethod &QtMetaProperty::notifySignal() const
{
    return signal_;
}

Value QtMetaProperty::read(const Object *object) const
{
    return QtVariant::toValue(meta_.read(static_cast<QObject const *>(object)));
}

bool QtMetaProperty::write(Object *object, Value &&value) const
{
    return meta_.write(static_cast<QObject *>(object), QtVariant::fromValue(value));
}

QtMetaMethod::QtMetaMethod(const QMetaMethod &meta)
    : meta_(meta)
{
}

const char *QtMetaMethod::name() const
{
    return meta_.name();
}

bool QtMetaMethod::isValid() const
{
    return meta_.isValid();
}

bool QtMetaMethod::isSignal() const
{
    return meta_.methodType() == QMetaMethod::Signal;
}

bool QtMetaMethod::isPublic() const
{
    return meta_.access() == QMetaMethod::Public;
}

size_t QtMetaMethod::methodIndex() const
{
    return static_cast<size_t>(meta_.methodIndex());
}

const char *QtMetaMethod::methodSignature() const
{
    return meta_.methodSignature();
}

Value::Type QtMetaMethod::returnType() const
{
    return QtVariant::type(meta_.returnType());
}

size_t QtMetaMethod::parameterCount() const
{
    return static_cast<size_t>(meta_.parameterCount());
}

Value::Type QtMetaMethod::parameterType(size_t index) const
{
    return QtVariant::type(meta_.parameterType(static_cast<int>(index)));
}

const char *QtMetaMethod::parameterName(size_t index) const
{
    return meta_.parameterNames().at(static_cast<int>(index));
}

struct VariantArgument
{
    operator QGenericArgument() const
    {
        if (!value.isValid()) {
            return QGenericArgument();
        }
        return QGenericArgument(value.typeName(), value.constData());
    }

    QVariant value;
};


bool QtMetaMethod::invoke(Object *object, Array &&args, Response const & resp) const
{
    // construct converter objects of QVariant to QGenericArgument
    VariantArgument arguments[10];
    for (int i = 0; i < qMin(static_cast<int>(args.size()), meta_.parameterCount()); ++i) {
        QVariant variant = QtVariant::fromValue(args.at(static_cast<size_t>(i)));
        variant.convert(meta_.parameterType(i));
        arguments[i].value = variant;
    }
    // construct QGenericReturnArgument
    QVariant returnValue;
    if (meta_.returnType() == QMetaType::Void) {
        // Skip return for void methods (prevents runtime warnings inside Qt), and allows
        // QMetaMethod to invoke void-returning methods on QObjects in a different thread.
        meta_.invoke(static_cast<QObject *>(object),
                  arguments[0], arguments[1], arguments[2], arguments[3], arguments[4],
                  arguments[5], arguments[6], arguments[7], arguments[8], arguments[9]);
    } else {
        // Only init variant with return type if its not a variant itself, which would
        // lead to nested variants which is not what we want.
        if (meta_.returnType() != QMetaType::QVariant)
            returnValue = QVariant(meta_.returnType(), nullptr);

        QGenericReturnArgument returnArgument(meta_.typeName(), returnValue.data());
        meta_.invoke(static_cast<QObject *>(object), returnArgument,
                  arguments[0], arguments[1], arguments[2], arguments[3], arguments[4],
                  arguments[5], arguments[6], arguments[7], arguments[8], arguments[9]);
    }
    // now we can call the method
    resp(QtVariant::toValue(returnValue));
    return true;
}

QtMetaEnum::QtMetaEnum(const QMetaEnum &meta)
    : meta_(meta)
{
}

const char *QtMetaEnum::name() const
{
    return meta_.name();
}

size_t QtMetaEnum::keyCount() const
{
    return static_cast<size_t>(meta_.keyCount());
}

const char *QtMetaEnum::key(size_t index) const
{
    return meta_.key(static_cast<int>(index));
}

int QtMetaEnum::value(size_t index) const
{
    return meta_.value(static_cast<int>(index));
}
