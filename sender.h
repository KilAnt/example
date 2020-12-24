#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>

class sender : public QObject
{
    Q_OBJECT
private:
    void startBroadcasting();

    QUdpSocket *socket;
public:
    explicit sender(QObject *parent = nullptr);
    virtual ~sender();
    void broadcastDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port);
signals:

public slots:
};

#endif // SENDER_H
