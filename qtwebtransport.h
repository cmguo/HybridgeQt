#ifndef QTWEBTRANSPORT_H
#define QTWEBTRANSPORT_H

#include "HybridgeQt_global.h"

#include <core/transport.h>

class QWebChannelAbstractTransport;
class QJsonObject;

class HYBRIDGEQT_EXPORT QtWebTransport : public Transport
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
