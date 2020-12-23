#ifndef VARIANT_H
#define VARIANT_H

#include "core/value.h"

#include <QVariant>

class Variant
{
public:
    static Value toValue(QVariant const & v);

    static QVariant fromValue(Value const & v);
};

#endif // VARIANT_H
