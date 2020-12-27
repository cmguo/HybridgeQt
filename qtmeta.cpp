#include "qtmeta.h"
#include "qtvariant.h"

#include <QMetaObject>
#include <QMetaProperty>

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

int QtMetaProperty::type() const
{
    return meta_.userType();
}

bool QtMetaProperty::isConstant() const
{
    return meta_.isConstant();
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

bool QtMetaProperty::write(Object *object, const Value &value) const
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

size_t QtMetaMethod::parameterCount() const
{
    return static_cast<size_t>(meta_.parameterCount());
}

int QtMetaMethod::parameterType(size_t index) const
{
    return meta_.parameterType(static_cast<int>(index));
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


Value QtMetaMethod::invoke(Object *object, const Array &args) const
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
    return QtVariant::toValue(returnValue);
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
