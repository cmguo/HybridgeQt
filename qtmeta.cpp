#include "qtmeta.h"
#include "variant.h"

#include <QMetaObject>
#include <QMetaProperty>

QtMetaObject::QtMetaObject(const QMetaObject &meta)
    : meta_(meta)
{
    for (int i = 0; i < meta_.enumeratorCount(); ++i) {
        metaEnums_.append(QtMetaEnum(meta_.enumerator(i)));
    }
    for (int i = 0; i < meta_.methodCount(); ++i) {
        metaMethods_.append(QtMetaMethod(meta_.method(i)));
    }
    for (int i = 0; i < meta_.propertyCount(); ++i) {
        metaProps_.append(QtMetaProperty(meta_.property(i), metaMethods_));
    }
}

const char *QtMetaObject::className() const
{
    return meta_.className();
}

int QtMetaObject::propertyCount() const
{
    return metaProps_.size();
}

const MetaProperty &QtMetaObject::property(int index) const
{
    return metaProps_.at(index);
}

int QtMetaObject::methodCount() const
{
    return metaMethods_.size();
}

const MetaMethod &QtMetaObject::method(int index) const
{
    return metaMethods_.at(index);
}

int QtMetaObject::enumeratorCount() const
{
    return metaEnums_.size();
}

const MetaEnum &QtMetaObject::enumerator(int index) const
{
    return metaEnums_.at(index);
}

static QtMetaMethod emptyMethod = QMetaMethod();

QtMetaProperty::QtMetaProperty(const QMetaProperty &meta, QVector<QtMetaMethod> const & methods)
    : meta_(meta)
    , signal_(meta.hasNotifySignal() ? methods.at(meta.notifySignalIndex()) : emptyMethod)
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

int QtMetaProperty::notifySignalIndex() const
{
    return meta_.notifySignalIndex();
}

const MetaMethod &QtMetaProperty::notifySignal() const
{
    return signal_;
}

Value QtMetaProperty::read(const Object *object) const
{
    return Variant::toValue(meta_.read(static_cast<QObject const *>(object)));
}

bool QtMetaProperty::write(Object *object, const Value &value) const
{
    return meta_.write(static_cast<QObject *>(object), Variant::fromValue(value));
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

int QtMetaMethod::methodIndex() const
{
    return meta_.methodIndex();
}

const char *QtMetaMethod::methodSignature() const
{
    return meta_.methodSignature();
}

int QtMetaMethod::parameterCount() const
{
    return meta_.parameterCount();
}

int QtMetaMethod::parameterType(int index) const
{
    return meta_.parameterType(index);
}

const char *QtMetaMethod::parameterName(int index) const
{
    return meta_.parameterNames().at(index);
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
        QVariant variant = Variant::fromValue(args.at(static_cast<size_t>(i)));
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
    return Variant::toValue(returnValue);
}

QtMetaEnum::QtMetaEnum(const QMetaEnum &meta)
    : meta_(meta)
{
}

const char *QtMetaEnum::name() const
{
    return meta_.name();
}

int QtMetaEnum::keyCount() const
{
    return meta_.keyCount();
}

const char *QtMetaEnum::key(int index) const
{
    return meta_.key(index);
}

int QtMetaEnum::value(int index) const
{
    return meta_.value(index);
}
