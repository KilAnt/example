#include "abstract_device.h"


abstract_device::abstract_device(QJsonArray _textsArray, QString _id, quint16 _port_r, quint16 _port_s, QObject *parent)
    : QObject(parent), textsArray(_textsArray), id(_id), port_r (_port_r), port_s(_port_s)
{
    qRegisterMetaType<message_to_ui>();
    send = new class sender(this);
    recive = new reciver(port_r, this);
    connect(&timerInit, &QTimer::timeout, this, &abstract_device::sendMess);
    connect(&timerQuery, &QTimer::timeout, this, &abstract_device::query_data);
    connect(recive, SIGNAL(getData(QByteArray &)), this, SLOT(json_recive(QByteArray &)));
    connect(this, SIGNAL(unknowJSON(QJsonDocument)), this, SLOT(detect_command(QJsonDocument)));
}

abstract_device::~abstract_device()
{
    delete send;
    delete recive;
}

void abstract_device::startBroadcasting()
{
    timerInit.start(500);
}

void abstract_device::setting_device(QJsonArray _textsArray, QString _id, quint16 _port_r, quint16 _port_s, qint8 _dev_error, qint8 _dev_power, QString _dev_status)
{
    textsArray = _textsArray;
    id = _id;
    port_r = _port_r;
    port_s = _port_s;
    init.error = _dev_error;
    init.power = _dev_power;
    init.status = _dev_status;

}

void abstract_device::sendMess()
{
    init_Device(id, dev_error_NO, dev_power_YES, dev_ready, port_r, port_s);
}

QString abstract_device::init_Device(const QString idDev, qint8 &error, qint8 &power, const QString status, quint16 port_r, quint16 port_s)
{
    QJsonDocument document_send;
    QJsonObject JsonObject;
    QByteArray m_JSON;
    QJsonObject start_mess;
    QJsonParseError ErrorJSON;
    QString return_struct;
    init.id = idDev;
    init.error = error;
    init.power = power;
    init.status = status;
    init.port_r = port_r;
    start_mess.insert("id", init.id);
    start_mess.insert("error", init.error);
    start_mess.insert("power", init.power);
    start_mess.insert("status", init.status);
    start_mess.insert("port_rec", init.port_r);
    QJsonArray textsArray = JsonObject["init"].toArray();
    textsArray.append(start_mess);
    JsonObject["init"] = textsArray;
    m_JSON = QJsonDocument(JsonObject).toJson(QJsonDocument::Indented);
    document_send = QJsonDocument::fromJson(m_JSON, &ErrorJSON);
    return_struct = document_send.toJson(QJsonDocument::Indented);
    QByteArray sendArr = QJsonDocument(JsonObject).toJson(QJsonDocument::Indented);
    send->broadcastDatagram(sendArr, QHostAddress::LocalHost, port_s);
    //qDebug()<<init.id;
    return return_struct;
}

void abstract_device::json_recive(QByteArray &datagram)
{
    QJsonDocument document_rec;
    QJsonObject obj;
    document_rec = QJsonDocument::fromJson(datagram, &ErrorJSON);
    emit unknowJSON(document_rec);
}

QString abstract_device::query_data()
{
    QJsonObject data;
    data["query"] = textsArray;
    QByteArray sendArr = QJsonDocument(data).toJson(QJsonDocument::Indented);
    send->broadcastDatagram(sendArr, QHostAddress::LocalHost, port_s);
    QString returned_mess = QJsonDocument(data).toJson(QJsonDocument::Indented);
    //qDebug()<< returned_mess;
    return returned_mess;
}

void abstract_device::ResetInitialize()
{
    timerQuery.stop();
    timerInit.start();
}

void abstract_device::CloseApp()
{
     qApp->quit();
}

void abstract_device::ResetApp()
{
    QTimer::singleShot(500,this,SLOT(startBroadcast()));
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void abstract_device::detect_command(QJsonDocument doc)
{
    if(doc.object().value("init").toArray().last().toString()=="ok")
    {
        timerInit.stop();
        timerQuery.start(100);
        qDebug()<<"init";
        emit ui_signal(INIT);
    }else
        if(doc.object().keys().last()== "data") {
            QJsonObject json_data = doc.object();
            json_to_struct(json_data);
            emit ui_signal(QUERY);
        }else if (doc.object().value("control").toArray().last().toString()=="restart") {
            emit ui_signal(RESTART);
        }else if (doc.object().value("control").toArray().last().toString()=="close") {
            emit ui_signal(CLOSE);
        }else
            if(doc.object().value("init").toArray().last().toString()=="stop"){
                timerInit.start(500);
                timerQuery.stop();
            }else    qDebug()<<"dont init";

}
