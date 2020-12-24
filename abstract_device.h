#ifndef ABSTRACT_DEVICE_H
#define ABSTRACT_DEVICE_H

#include <QObject>
#include <QTimer>
#include "reciver.h"
#include "sender.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QList>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QApplication>
#include <QProcess>

struct struct_init
{
    QString id;
    qint8 power;
    qint8 error;
    QString status;
    quint16 port_r;
};

enum message_to_ui
{
    INIT,
    QUERY,
    RESTART,
    CLOSE
};

Q_DECLARE_METATYPE(message_to_ui)

/**
 * @brief The abstract_device class
 * Абстрактный класс подключаемого устройства. Содежит основные методы,
 * необходимые для подключения, инициализации и информационного обмена с системой.
 */

class abstract_device : public QObject
{
    Q_OBJECT
public:
    ///
    /// \brief abstract_device
    /// \param _textsArray - Массив параметров, запрашиваемых у сервера
    /// \param _id  - обозначение устройства
    /// \param _port_r - Порт, на который устройство принимает данные
    /// \param _port_s - Порт, на который устройство отправляет данные
    /// \param parent  - Родитель объекта
    ///
    explicit abstract_device(QJsonArray _textsArray, QString _id, quint16 _port_r, quint16 _port_s, QObject *parent = nullptr);
    virtual ~abstract_device();

    ///
    /// \brief setting_device - Производит изменение стартовых параметров параметров устройства
    /// \param _textsArray
    /// \param _id
    /// \param _port_r  - порт, на который приходят данные из системы
    /// \param _port_s - порт, на который отправляются данные в систему
    /// \param _dev_error - указание наличия ошибок в работе
    /// \param _dev_power - включено ли устройство
    /// \param _dev_status - статус устройства
    ///
    void setting_device(QJsonArray _textsArray, QString _id, quint16 _port_r, quint16 _port_s, qint8 _dev_error, qint8 _dev_power, QString _dev_status);

    ///
    /// \brief startBroadcasting
    /// Запускает отправку пакета инициализации
    ///
    void startBroadcasting();
    ///Объект отправки данных
    class sender *send;
    ///Объект получения данных
    class reciver *recive;
    ///
    /// \brief ResetApp Перезапуск приложения
    ///
    void ResetApp();
    ///
    /// \brief CloseApp Закрытие приложения
    ///
    void CloseApp();
    ///Ошибка в устройстве отсутствует
    qint8 dev_error_NO = 0;
    ///Наличие ошибки устройства
    qint8 dev_error_YES = 1;
    ///Устройство не подключено
    qint8 dev_power_NO = 0;
    ///Устройство подключено
    qint8 dev_power_YES = 1;
    ///Устройство готово к работе
    const QString dev_ready = "ready";
    ///Устройство прекращает работу
    const QString dev_end = "end";
    ///Неверные данные
    const QString dev_dataErr = "dataErr";
signals:
    ///
    /// \brief unknowJSON - испускается при получении JSON
    /// \param doc - JSON-сигнал
    ///
    void unknowJSON(QJsonDocument doc);
    ///
    /// \brief json_to_struct - испускается при обнаружении данных из обсервера
    /// \param object - данные из обсервера
    ///
    void json_to_struct(QJsonObject &object);
    void ui_signal(message_to_ui message);
private:
    /// Лист запросов данных для работы устройства
    QJsonArray textsArray;
    /// ID - устройства
    QString id;

    ///Порт, на который приходят данные из системы
    quint16 port_r = NULL;
    ///Порт, на который отправляются данные в систему
    quint16 port_s = NULL;
    ///Директория, в котрой расположен JSON-файл
    QString saveFileName;
    ///Структура с параметрами инициализации
    struct_init init;
    ///Объект для отслеживания ошибок
    QJsonParseError ErrorJSON;

    ///
    /// \brief sendMess
    ///Слот для отправки пакета инициализации
    ///
    void sendMess();
    ///
    /// \brief init_Device
    /// Отправка JSON-пакета инициализации.
    ///
    /// \param idDev - ID устрйоства.
    /// \param error - наличие/отсутствие ошибки
    /// \param power - вкл/выкл устройства
    /// \param status - состояние устройства
    /// \param port_r - порт, на который мы ожидаем принять данные из обсервера
    /// \param port_s - порт обсервера
    /// \return Возвращает отправленную JSON-структуру
    ///
    QString init_Device(const QString idDev, qint8 &error, qint8 &power, const QString status, quint16 port_r, quint16 port_s);
    ///Таймер для отправки структуры инициализации
    QTimer timerInit;
    ///Таймер для отправки структуры запроса
    QTimer timerQuery;
    ///
    /// \brief query_data - отправка списка запрашиваемых данных в ОBS
    /// \param list - список запрашиваемых данных
    /// \return Возвращает отправленную JSON-структуру
    ///
    QString query_data();
public slots:
    ///
    /// \brief ResetInitialize - Производит остановку таймера для отправки query, запуск таймера init
    ///
    void ResetInitialize();
private slots:
    ///
    /// \brief json_recive - Преобразует датаграму из сокета в QJsonDocument и испускает с ним сигнал
    /// \param datagram датаграма из сокета
    ///
    void json_recive(QByteArray &datagram);
    ///
    /// \brief detect_command - Определяет что за json-пакет пришел
    /// \param doc - json-пакет
    ///
    void detect_command(QJsonDocument doc);
};

#endif // ABSTRACT_DEVICE_H
