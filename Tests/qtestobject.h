#ifndef QTESTOBJECT_H
#define QTESTOBJECT_H

#include <QObject>

class QTestObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int value MEMBER value_ NOTIFY valueChanged)

public:
    QTestObject();

public slots:
    int testMethod(int x, int y);

signals:
    void valueChanged();

private:
    int value_ = 0;
};

#endif // QTESTOBJECT_H
