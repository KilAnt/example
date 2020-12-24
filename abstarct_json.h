#ifndef ABSTARCT_JSON_H
#define ABSTARCT_JSON_H

#include <QObject>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>


class Abstarct_json : public QObject
{
    Q_OBJECT
private:
    inline void deleteTextJSON();
    QJsonParseError ErrorJSON;
    QJsonDocument JsonDoc;
    QByteArray docJSON;
    QJsonDocument document;
public:
    explicit Abstarct_json(QObject *parent = nullptr);
    virtual ~Abstarct_json();
    virtual void write(const QJsonObject &obj, const QString filePath);
    virtual QJsonDocument read(const QString filePath);
    virtual void deleteTextJSON(const QString saveFileName);
signals:

public slots:
};

#endif // ABSTARCT_JSON_H
