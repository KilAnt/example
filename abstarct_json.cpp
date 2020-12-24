#include "abstarct_json.h"

Abstarct_json::Abstarct_json(QObject *parent) : QObject(parent)
{

}

Abstarct_json::~Abstarct_json()
{

}

void Abstarct_json::write(const QJsonObject &obj, const QString filePath)
{
    QFile jsonFile(filePath);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    jsonFile.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    jsonFile.close();
}

QJsonDocument Abstarct_json::read(const QString filePath)
{
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly))
      {
         document = QJsonDocument::fromJson(QByteArray(file.readAll()), &ErrorJSON);
      }
    file.close();
    return document;
}


void Abstarct_json::deleteTextJSON(const QString saveFileName)
{
    QJsonObject deleteObj;
    deleteObj.insert("","");
    write(deleteObj, saveFileName);
}
