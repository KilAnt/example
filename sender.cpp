#include "sender.h"

sender::sender(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
}

sender::~sender()
{
    delete socket;
}

void sender::broadcastDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
    socket->writeDatagram(datagram, host, port);
}
