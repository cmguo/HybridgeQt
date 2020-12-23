#ifndef QTWEBTRANSPORT_H
#define QTWEBTRANSPORT_H

#include <core/transport.h>

class QWebChannelAbstractTransport;
class QJsonObject;

class QtWebTransport : public Transport
{
public:
    QtWebTransport(QWebChannelAbstractTransport * transport);

    // Transport interface
public:
    virtual void sendMessage(const Message &message) override;

private:
    void messageReceived(const QJsonObject &message, QWebChannelAbstractTransport *transport);

private:
    QWebChannelAbstractTransport * transport_ = nullptr;
};

#endif // QTWEBTRANSPORT_H
