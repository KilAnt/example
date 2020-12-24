#include "multikreader.h"
#include <QFile>
#include <QDebug>
//------------------------------------------------------------------------------------------------------------------------------------------------------------

void FileReader::setPath(const QString &value)
{
    path = value;
}

void FileReader::ReadFile()
{
    fileToRead.setFileName(path);
    if(fileToRead.open(QFile::ReadOnly))
    {
    readedText = fileToRead.readAll();
    fileToRead.close();
    header=QJsonDocument::fromJson(readedText.toUtf8()).object().find("id_полёта").value().toString();
    readJArr=QJsonDocument::fromJson(readedText.toUtf8()).object().find("data").value().toArray();
    emit sendText(&header,&readJArr);
    emit done();
    }
    else
    {
       qDebug()<<"Файл недоступен!";
       emit done();
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------

MultikReader::MultikReader()
{
    connect(&readTimer,&QTimer::timeout,this,&MultikReader::onReadTimerTick);
    connect(this,SIGNAL(readed()),this,SLOT(startTimer()));
}

void MultikReader::Initise()
{
    if(!isInitised())
    {
        readFile();
    }
    else
        qDebug()<<"Reader already initised";
}

void MultikReader::readFile()
{
    reader= new FileReader();
    reader->setPath(readFileName);
    reader->moveToThread(&readThread);
    connect(&readThread,SIGNAL(started()),reader,SLOT(ReadFile()));
    connect(reader,SIGNAL(done()),&readThread,SLOT(quit()));
    connect(reader,SIGNAL(sendText(QString*,QJsonArray*)),this,SLOT(onText(QString*,QJsonArray*)));
    readThread.start();
}
void MultikReader::onText(QString* head,QJsonArray* readedJArr)
{
    header=*head;
    readJArr=*readedJArr;
    foreach (QJsonValue val, readJArr)
    {
        tickFramesVector.append(val.toObject().find("timeFrame").value().toInt());
        if(val.toObject().contains("1_timeGNSS"))
        {
            int t = val.toObject().find("1_timeGNSS").value().toInt();
            lastTime= QTime(t/10000000,t/100000%100,t/1000%100,t%1000);
        }
        timeVector.append(lastTime);
    }
    currentTick=0;
    reader->deleteLater();
    emit readed();
}

void MultikReader::startTimer()
{
    readTimer.start();
    isRun=true;
};

void MultikReader::Stop()
{
    readTimer.stop();
    isRun=false;
    tickFramesVector.clear();
    timeVector.clear();
    readJArr=QJsonArray();

    currentTick=0;
}

void MultikReader::onReadTimerTick()
{
    QJsonObject curJObj=readJArr[currentTick].toObject();

    curJObj=packInDatadev(curJObj);
    if(isRunning())
    {
        if(currentTick==readJArr.count()-1)
        {
            currentTick=0;
        }
        else
        {
            int interval=tickFramesVector[currentTick+1]-tickFramesVector[currentTick];
            readTimer.setInterval(interval);
            currentTick++;
        }
    }
    emit currentFrame(currentTick);
    emit sendData(JObjToByteArr(curJObj));
}

MultikReader::~MultikReader()
{
    Stop();
}

//----------------------------------------------------------------------------------------------------------------------------------
void MultikReader::Pause()
{
    if(isInitised()&&isRunning())
        isRun=false;
    else
    {
        qDebug()<<"Reader not started or not running";
    }
}

void MultikReader::Continue()
{
    if(isInitised()&&!isRunning())
        isRun=true;
    else
    {
        qDebug()<<"Reader not started or already running";
    }
}

void MultikReader::setFrame(uint value)
{
    if(isInitised())
    {
        if(value<readJArr.count())
            currentTick=value;
        else
            currentTick=0;
    }
    else
        qDebug()<<"Reader not started";
}

QJsonObject MultikReader::packInDatadev(QJsonObject object)
{
    QJsonArray arr;
    QJsonObject obj;
    arr.append(object);
    obj.insert("datadev",object);
    return obj;
}

QByteArray MultikReader::JObjToByteArr(QJsonObject obj)
{
    return QJsonDocument(obj).toJson(QJsonDocument::Indented);
}

QString MultikReader::getHeader() const
{
    return header;
}

void MultikReader::setReadFileName(const QString &value)
{
    readFileName = value;
}

QVector<uint>* MultikReader::getTickVector()
{
    return &tickFramesVector;
}

QVector<QTime> *MultikReader::getTimeVector()
{
    return &timeVector;
}

bool MultikReader::isInitised() const
{
    return readJArr.count();
}

bool MultikReader::isRunning() const
{
    return isRun;
}

uint MultikReader::getFirstFrame() const
{
    if(isInitised())
    {
        return tickFramesVector.first();
    }
    else return 0;
}

uint MultikReader::getLastFrame() const
{
    if(isInitised())
    {
        return tickFramesVector.last();
    }
    else return 0;
}

QTime MultikReader::getFirstTime() const
{
    if(isInitised())
    {
        int i=0;
        while (timeVector[i].isNull())
            i++;
        return timeVector[i];
    }
    else return QTime();
}

QTime MultikReader::getLastTime() const
{
    if(isInitised())
    {
        return timeVector.last();
    }
    else return QTime();
}


