#include "qtestobject.h"

QTestObject::QTestObject()
{
    setObjectName("testObject");
}

int QTestObject::testMethod(int x, int y)
{
    return x * x + y * y;
}
