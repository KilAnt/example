#ifndef RECIVER_H
#define RECIVER_H

#include <QUdpSocket>
#include <QObject>

class reciver : public QObject
{
    Q_OBJECT
private:
    QUdpSocket *socket;
    QByteArray datagram;
        int count = 0;
public:
    explicit reciver(quint16 port, QObject *parent = nullptr);
    virtual ~reciver();
    quint16 _port;
signals:
    QByteArray getData(QByteArray &datagram);
public slots:
    QByteArray recive();
};

#endif // RECIVER_H
