#ifndef JSONREADER_H
#define JSONREADER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFile>
#include <QDir>
#include <QVector>

//----------------------------------------------------------------------------------------------------------------------
class FileReader : public QObject
{
    Q_OBJECT
public:
    explicit FileReader(){};
    void setPath(const QString &path);
signals:
    void sendText(QString*,QJsonArray*);
    void done();
public slots:
    void ReadFile();
private:
    QFile fileToRead;
    QString path;
    QString readedText;
    QString header;
    QJsonArray readJArr;
};
//----------------------------------------------------------------------------------------------------------------------

class MultikReader : public QObject
{
    Q_OBJECT

public:
    explicit MultikReader();
    /** @brief Установка читаемого файла. Устанавливать перед запуском */
    void setReadFileName(const QString &value);

    /** @brief Возващет True если файл прочитан и запущена отправка*/
    bool isInitised() const;
    /** @brief Возващет True если идёт перебор кадров*/
    bool isRunning() const;

    /** @brief Установка текущего кадра, при отсутствии данного кадра выбирается ближайший следующий. Работает после запуска. */
    void setFrame(const uint value);

    /** @brief Установка текущего кадра, при отсутствии данного кадра выбирается ближайший следующий. Работает после запуска. */
    void setTime(QTime value);

    /** @brief Возвращает вектор с TimeFrame текущего файла. Работает после запуска. */
    QVector<uint> *getTickVector();
    /** @brief Возвращает вектор с GNSS временем текущего файла. Работает после запуска. */
    QVector<QTime> *getTimeVector();
    /** @brief Возващет первое значение TimeFrame из файла*/
    uint getFirstFrame() const;
    /** @brief Возващет последнее значение TimeFrame из файла*/
    uint getLastFrame() const;
    /** @brief Возващет первое значение TimeGNSS из файла*/
    QTime getFirstTime() const;
    /** @brief Возващет последнее значение TimeGNSS из файла*/
    QTime getLastTime() const;
    /** @brief Возващет id_полёта из файла*/
    QString getHeader() const;
    ~MultikReader();

public slots:
    /** @brief Чтение файла в память и запуск отправки стурктур*/
    void Initise();
    /** @brief Остановка передачи данных и удаление файла из памяти*/
    void Stop();
    /** @brief Остановка перебора кадров с продолжением отправки текущего кадра*/
    void Pause();
    /** @brief Продолжение перебора кадров*/
    void Continue();
signals:
    /** @brief Сигнал с JSON структурой текущего timeFrame
    */
    void sendData(QByteArray);

    /** @brief Сигнал с значением времени текущего кадра
    *	@param[out] int текущий TimeFrame
    */
    void currentFrame(int);

    void readed();

private slots:

    void onText(QString *,QJsonArray*);
    void startTimer();

private:
    QJsonArray readJArr;    //!< Массив с данными по всем кадрам
    bool isRun=false;       //!< Отвечает за перебор кадров
    QString header;         //!< Id_полёта
    QTimer readTimer;       //!< Таймер отправки данных
    QString readFileName;   //!< Полное имя читаемого файла
    QVector<uint> tickFramesVector;  //!< Вектор с TimeFrame'ами текущего файла
    QTime lastTime;
    QVector<QTime> timeVector;  //!< Вектор с GNSS временем текущего файла
    int currentTick=0;      //!< Текущее положение в векторе


    /** @brief Чтение файла из readFileName в readJArr с заполнением tickFramesVector и header*/
    void readFile();

    /** @brief Отправка структур по таймеру, перебор если IsRun*/
    void onReadTimerTick();

    /** @brief обёртка JSON обьекта в datadev */
    QJsonObject packInDatadev(QJsonObject object);
    /** @brief Перевод JObject в ByteArray*/
    QByteArray JObjToByteArr(QJsonObject);

    FileReader * reader;
    QThread readThread;
};
#endif // JSONREADER_H
