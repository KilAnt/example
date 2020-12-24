#include "reciver.h"
#include <QDebug>

reciver::reciver(quint16 port, QObject *parent) : QObject(parent), _port(port)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, _port);
    connect(socket, SIGNAL(readyRead()), this, SLOT(recive()));
}

reciver::~reciver()
{
    delete socket;
}

QByteArray reciver::recive()
{
    qDebug()<< socket->state();
    while(socket->hasPendingDatagrams())
    {
    qDebug()<<"state#2" <<socket->state();
        datagram.resize(int(socket->pendingDatagramSize()));
            qDebug()<<"state#3" <<socket->state();
        socket->readDatagram(datagram.data(), datagram.size());
    qDebug()<<"state#4" <<socket->state();
        emit getData(datagram);
    }
    qDebug()<<"state#5" <<socket->state();
    return datagram;
}
