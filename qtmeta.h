#ifndef QTMETA_H
#define QTMETA_H

#include <core/object.h>

#include <QMetaProperty>
#include <QVector>

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
    virtual int propertyCount() const override;
    virtual const MetaProperty &property(int index) const override;
    virtual int methodCount() const override;
    virtual const MetaMethod &method(int index) const override;
    virtual int enumeratorCount() const override;
    virtual const MetaEnum &enumerator(int index) const override;

private:
    QMetaObject const & meta_;
    QVector<QtMetaProperty> metaProps_;
    QVector<QtMetaMethod> metaMethods_;
    QVector<QtMetaEnum> metaEnums_;
};

class QMetaProperty;

class QtMetaProperty : public MetaProperty
{
public:
    QtMetaProperty(QMetaProperty const & meta = QMetaProperty(), QVector<QtMetaMethod> const & methods = {});

    // MetaProperty interface
public:
    virtual const char *name() const override;
    virtual bool isValid() const override;
    virtual int type() const override;
    virtual bool isConstant() const override;
    virtual bool hasNotifySignal() const override;
    virtual int notifySignalIndex() const override;
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
    virtual int methodIndex() const override;
    virtual const char *methodSignature() const override;
    virtual int parameterCount() const override;
    virtual int parameterType(int index) const override;
    virtual const char *parameterName(int index) const override;
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
    virtual int keyCount() const override;
    virtual const char *key(int index) const override;
    virtual int value(int index) const override;

private:
    QMetaEnum meta_;
};

#endif // QTMETA_H
