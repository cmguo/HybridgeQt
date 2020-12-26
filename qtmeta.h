#ifndef QTMETA_H
#define QTMETA_H

#include <core/object.h>

#include <QMetaProperty>

#include <vector>

class QtMetaProperty;
class QtMetaMethod;
class QtMetaEnum;

struct QMetaObject;

class QtMetaObject : public MetaObject
{
public:
    QtMetaObject(QMetaObject const & meta);

    // MetaObject interface
public:
    virtual const char *className() const override;
    virtual size_t propertyCount() const override;
    virtual const MetaProperty &property(size_t index) const override;
    virtual size_t methodCount() const override;
    virtual const MetaMethod &method(size_t index) const override;
    virtual size_t enumeratorCount() const override;
    virtual const MetaEnum &enumerator(size_t index) const override;

private:
    QMetaObject const & meta_;
    std::vector<QtMetaProperty> metaProps_;
    std::vector<QtMetaMethod> metaMethods_;
    std::vector<QtMetaEnum> metaEnums_;
};

class QMetaProperty;

class QtMetaProperty : public MetaProperty
{
public:
    QtMetaProperty(QMetaProperty const & meta = QMetaProperty(), std::vector<QtMetaMethod> const & methods = {});

    // MetaProperty interface
public:
    virtual const char *name() const override;
    virtual bool isValid() const override;
    virtual int type() const override;
    virtual bool isConstant() const override;
    virtual bool hasNotifySignal() const override;
    virtual size_t notifySignalIndex() const override;
    virtual const MetaMethod &notifySignal() const override;
    virtual Value read(const Object *object) const override;
    virtual bool write(Object *object, const Value &value) const override;

private:
    QMetaProperty meta_;
    QtMetaMethod const & signal_;
};

class QtMetaMethod : public MetaMethod
{
public:
    QtMetaMethod(QMetaMethod const & meta = QMetaMethod());

    // MetaMethod interface
public:
    virtual const char *name() const override;
    virtual bool isValid() const override;
    virtual bool isSignal() const override;
    virtual bool isPublic() const override;
    virtual size_t methodIndex() const override;
    virtual const char *methodSignature() const override;
    virtual size_t parameterCount() const override;
    virtual int parameterType(size_t index) const override;
    virtual const char *parameterName(size_t index) const override;
    virtual Value invoke(Object *object, const Array &args) const override;

private:
    QMetaMethod meta_;
};

class QtMetaEnum : public MetaEnum
{
public:
    QtMetaEnum(QMetaEnum const & meta = QMetaEnum());

    // MetaEnum interface
public:
    virtual const char *name() const override;
    virtual size_t keyCount() const override;
    virtual const char *key(size_t index) const override;
    virtual int value(size_t index) const override;

private:
    QMetaEnum meta_;
};

#endif // QTMETA_H
