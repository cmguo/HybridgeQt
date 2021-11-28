#ifndef VARIANT_H
#define VARIANT_H

#include "core/value.h"

#include <QVariant>

class QtVariant
{
public:
    static Value::Type type(int type);

    static int type(Value::Type type);

    static Value toValue(QVariant const & v);

    static QVariant fromValue(Value const & v);
};

#endif // VARIANT_H
