#include "qtvariant.h"

Value QtVariant::toValue(const QVariant &v)
{
    if (v.isNull())
        return Value();
    if (QMetaType::typeFlags(v.userType()).testFlag(QMetaType::PointerToQObject))
        return v.value<QObject*>();
    switch (v.userType()) {
    case QVariant::Int:
    case QVariant::UInt:
        return v.toInt();
    case QVariant::LongLong:
    case QVariant::ULongLong:
        return v.toLongLong();
    case QMetaType::Float:
        return v.toFloat();
    case QVariant::Double:
        return v.toDouble();
    case QVariant::Bool:
        return v.toBool();
    case QVariant::ByteArray:
    case QVariant::String:
        return v.toByteArray().data();
    case QVariant::List:
    case QVariant::StringList:
    case QMetaType::QJsonArray:
    {
        Array a;
        auto l = v.toList();
        for (auto & i : l)
            a.emplace_back(toValue(i));
        return std::move(a);
    }
    case QVariant::Map:
    case QMetaType::QVariantHash:
    case QMetaType::QJsonObject:
    {
        Map a;
        auto m = v.toMap();
        for (auto i = m.begin(); i != m.end(); ++i)
            a.emplace(std::make_pair(i.key().toUtf8().data(), toValue(i.value())));
        return std::move(a);
    }
    }
    return Value();
}

QVariant QtVariant::fromValue(const Value &v)
{
    if (v.isInt())
        return v.toInt();
    else if (v.isLong())
        return v.toLong();
    else if (v.isFloat())
        return v.toFloat();
    else if (v.isDouble())
        return v.toDouble();
    else if (v.isString())
        return v.toString().c_str();
    else if (v.isBool())
        return v.toBool();
    else if (v.isArray()) {
        QVariantList l;
        Array const & a = v.toArray();
        for (auto & v : a) {
            l.append(fromValue(v));
        }
        return l;
    } else if (v.isMap()) {
        QVariantMap m;
        Map const & a = v.toMap();
        for (auto & v : a) {
            m.insert(v.first.c_str(), fromValue(v.second));
        }
        return m;
    } else if (v.isObject()) {
        return QVariant::fromValue(static_cast<QObject*>(v.toObject()));
    }
    return QVariant();
}
