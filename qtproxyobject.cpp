#include "qtproxyobject.h"
#include "qtvariant.h"

#include <QMetaMethod>
#include <metaobjectbuilder.h>

using namespace QtPromise;

const QtProxyObject::qt_meta_stringdata_JsObject_t QtProxyObject::qt_meta_stringdata_JsObject = {};
const uint QtProxyObject::qt_meta_data_JsObject[] = {{}};

const QMetaObject QtProxyObject::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_JsObject.data,
      qt_meta_data_JsObject, qt_static_metacall, nullptr, nullptr }
};

QtProxyObject::QtProxyObject(Map &&classinfo)
    : ProxyObject(std::move(classinfo))
    , metaobj_(new QtProxyMetaObject(metaObj()))
{
}

QVariant QtProxyObject::readProperty(const QByteArray &property)
{
    MetaProperty const * mp = ProxyObject::property(property);
    if (mp) {
        Value v = mp->read(this);
        return QtVariant::fromValue(v);
    }
    return QVariant();
}

bool QtProxyObject::setProperty(QByteArray const & property, const QVariant &value)
{
    MetaProperty const * mp = ProxyObject::property(property);
    if (mp) {
        return mp->write(this, QtVariant::toValue(value));
    }
    return false;
}

QPromise<QVariant> QtProxyObject::invokeMethod(QByteArray const & method, const QVariantList &args)
{
    MetaMethod const * md = ProxyObject::method(method);
    if (md == nullptr)
        return QPromise<QVariant>::reject(std::runtime_error("no such method"));
    return QPromise<QVariant>([this, md, &args] (QPromiseResolve<QVariant> resolve, QPromiseReject<QVariant> reject) {
        Array argv;
        for (size_t j = 0; j < md->parameterCount(); ++j)
            argv.emplace_back(QtVariant::toValue(args[static_cast<int>(j)]));
        bool ok = md->invoke(this, std::move(argv), [=](Value && result) {
            resolve(QtVariant::fromValue(result));
        });
        if (!ok) {
            reject(std::runtime_error("!ok"));
        }
    });
}

bool QtProxyObject::connectToSignal(int signalIndex)
{
    MetaObject const * meta = metaObj();
    size_t index = static_cast<size_t>(signalIndex);
    if (index >= meta->methodCount())
        return false;
    MetaMethod const & md = meta->method(index);
    if (!md.isSignal())
        return false;
    return metaObj()->connect(MetaObject::Connection(this, index, this, [](
                                              void * receiver, Object const * object, size_t index, Array && args) {
        QtProxyObject * proxy = reinterpret_cast<QtProxyObject*>(receiver);
        QVariantList argv;
        for (size_t j = 0; j < args.size(); ++j)
            argv.append(QtVariant::fromValue(args[j]));
        proxy->signalEmitted(static_cast<int>(index), argv);
    }));
}

bool QtProxyObject::disconnectFromSignal(int signalIndex)
{
    size_t index = static_cast<size_t>(signalIndex);
    return metaObj()->connect(MetaObject::Connection(this, index, this, nullptr));
}

void QtProxyObject::connectNotify(const QMetaMethod &signal)
{
    QByteArray signature = signal.methodSignature();
    signature.prepend("2"); // SIGNAL();
    if (receivers(signature) == 1)
        connectToSignal(signal.methodIndex());
}

void QtProxyObject::disconnectNotify(const QMetaMethod &signal)
{
    if (!isSignalConnected(signal))
        disconnectFromSignal(signal.methodIndex());
}

void QtProxyObject::signalEmitted(int index, const QVariantList &args)
{
    QMetaMethod sig = metaobj_->method(index);
    if (!isSignalConnected(sig))
        return;
    QVector<void*> v(sig.parameterCount(), nullptr);
    QVariantList list = args;
    for (int i = 0; i < list.size() && i < v.size(); ++i) {
        if (!list[i].convert(sig.parameterType(i)))
            qWarning() << "signalEmitted";
        v[i] = list[i].data();
    }
    QMetaObject::activate(this, metaobj_, index - metaobj_->methodOffset(), v.data());
}

int QtProxyObject::internalProperty(QMetaObject::Call call, int index, void **v)
{
    QMetaProperty prop = metaobj_->property(index + metaobj_->propertyOffset());
    index -= metaobj_->propertyCount();
    switch (call) {
    case QMetaObject::ReadProperty: {
        QVariant value = readProperty(prop.name());
        QMetaType::construct(prop.userType(), v, value.constData());
    }
        break;
    case QMetaObject::WriteProperty:
    {
        QVariant var(prop.userType(), v[0]);
        setProperty(prop.name(), var);
        break;
    }
    case QMetaObject::ResetProperty:
        break;
    default:
        break;
    }
    return index;
}

int QtProxyObject::internalInvoke(int index, void **v)
{
    QMetaMethod method = metaobj_->method(index + metaobj_->methodOffset());
    index -= metaobj_->methodCount();
    switch (method.methodType()) {
    case QMetaMethod::Signal:
        QMetaObject::activate(this, metaobj_, index, v);
        return index - metaobj_->methodCount();
    case QMetaMethod::Method:
    case QMetaMethod::Slot:
    {
        QVariantList args;
        for (int i = 0; i < method.parameterCount(); ++i) {
            args.append(QVariant(method.parameterType(i), v[i]));
        }
        invokeMethod(method.name(), args);
        return index;
    }
    default:
        break;
    }
    return index;
}

const QMetaObject * QtProxyObject::metaObject() const
{
    return metaobj_;
}

void * QtProxyObject::qt_metacast(const char * cname)
{
    if (!qstrcmp(cname, "JsObject"))
        return static_cast<void *>(this);
    return QObject::qt_metacast(cname);
}

void QtProxyObject::qt_static_metacall(QObject * t, QMetaObject::Call call, int id, void ** v)
{
    if (call == QMetaObject::InvokeMetaMethod) {
        static_cast<QtProxyObject*>(t)->internalInvoke(id, v);
    }
}

int QtProxyObject::qt_metacall(QMetaObject::Call call, int id, void **v)
{
    id = QObject::qt_metacall(call, id, v);
    if (id < 0)
        return id;
    const QMetaObject *mo = metaObject();
    switch(call) {
    case QMetaObject::InvokeMetaMethod:
        id = internalInvoke(id, v);
        break;
    case QMetaObject::ReadProperty:
    case QMetaObject::WriteProperty:
    case QMetaObject::ResetProperty:
        id = internalProperty(call, id, v);
        break;
    case QMetaObject::QueryPropertyScriptable:
    case QMetaObject::QueryPropertyDesignable:
    case QMetaObject::QueryPropertyStored:
    case QMetaObject::QueryPropertyEditable:
    case QMetaObject::QueryPropertyUser:
        id -= mo->propertyCount();
        break;
    default:
        break;
    }
    Q_ASSERT(id < 0);
    return id;
}

QtProxyMetaObject::QtProxyMetaObject(MetaObject * meta)
{
    d.data = nullptr;
    d.stringdata = nullptr;
    MetaObjectBuilder builder;
    // properties
    for (size_t i = 0; i < meta->propertyCount(); ++i) {
        MetaProperty const & mp = meta->property(i);
        //builder.addProperty(mp.type(), it.key().toUtf8(), 0);
    }
    // methods
    for (size_t i = 0; i < meta->methodCount(); ++i) {
        MetaMethod const & md = meta->method(i);
        //builder.addSlot(it.key().toUtf8(), 0);
        //builder.addSignal(it.key().toUtf8());
    }
    builder.buildMetaData(this);
}

QtProxyMetaObject::~QtProxyMetaObject()
{
    delete [] d.data;
    delete [] reinterpret_cast<char *>(const_cast<QByteArrayData *>(d.stringdata));
}
