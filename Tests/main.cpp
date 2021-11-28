#include "qtestobject.h"

#include <qtchannel.h>

#include <tool/pairedtransport.h>

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QtChannel channel;
    QTestObject * object = new QTestObject;
    channel.registerObject("test", object);
    auto pair = PairedTransport::createTransports();
    channel.connectTo(pair.first);
    QtChannel channel2;
    channel2.connectTo(pair.second, [](QMap<QByteArray, QObject*> objects) {
        qDebug() << "init result" << objects;
        QObject* object = objects.first();
        int x = 2, y = 3;
        int result = 0;
        object->metaObject()->invokeMethod(object, "testMethod", Q_RETURN_ARG(int,result),
                                           Q_ARG(int,x), Q_ARG(int,y));
    });
    a.exec();
}
