#ifndef JSOBJECT_H
#define JSOBJECT_H

#include <core/metaobject.h>
#include <core/proxyobject.h>

#include <QtPromise>

#include <QObject>
#include <QVariant>

#include <functional>

class RevertWebChannel;
class QtProxyMetaObject;

class QtProxyObject : public QObject, public ProxyObject
{
    Q_OBJECT_FAKE
public:
    explicit QtProxyObject(Map &&classinfo);

public:
    QVariant readProperty(QByteArray const & property);

    bool setProperty(QByteArray const & property, QVariant const & value);

    QtPromise::QPromise<QVariant> invokeMethod(QByteArray const & method, QVariantList const & args);

protected:
    bool connectToSignal(int signalIndex);

    bool disconnectFromSignal(int signalIndex);

    virtual void connectNotify(const QMetaMethod &signal) override;

    virtual void disconnectNotify(const QMetaMethod &signal) override;

private:
    struct qt_meta_stringdata_JsObject_t {
        QByteArrayData data[13];
        char stringdata[88];
    };
    static const qt_meta_stringdata_JsObject_t qt_meta_stringdata_JsObject;
    static const uint qt_meta_data_JsObject[];

private:
    QVariant unwrapQObject(QVariant const & object);

    void unwrapProperties();

private:
    friend class RevertWebChannel;

    int internalProperty(QMetaObject::Call call, int index, void **v);

    int internalInvoke(int index, void **v);

    void signalEmitted(int index, QVariantList const & args);

    void propertyUpdate(QVariantMap const & _signals, QVariantMap const & properties);

private:
    QByteArray const id_;
    QtProxyMetaObject const * metaobj_ = nullptr;
    QMap<QByteArray, QVariant> properties_;
    QMap<QByteArray, QVariant> signals_;
    //QVector<QVector<std::function<void(QVariantList)>> connections_;
};


class QtProxyMetaObject : public QMetaObject
{
public:
    QtProxyMetaObject(MetaObject * meta);

    ~QtProxyMetaObject();
};

template <> inline QtProxyObject *qobject_cast<QtProxyObject*>(const QObject *o)
{
    void *result = o ? const_cast<QObject *>(o)->qt_metacast("JsObject") : nullptr;
    return reinterpret_cast<QtProxyObject*>(result);
}
template <> inline QtProxyObject *qobject_cast<QtProxyObject*>(QObject *o)
{
    void *result = o ? o->qt_metacast("JsObject") : nullptr;
    return reinterpret_cast<QtProxyObject*>(result);
}


#endif // JSOBJECT_H