#ifndef JIKIMZON_EVENTLOGMANAGER_H
#define JIKIMZON_EVENTLOGMANAGER_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

class EventLogManager : public QObject
{
    Q_OBJECT
public:
    explicit EventLogManager(const QString &dbPath, QObject *parent = nullptr);
    ~EventLogManager();

    void createTable();
    void saveEventLog(QJsonObject& obj);

signals:

private:
    QSqlDatabase mDB;
};

#endif // JIKIMZON_EVENTLOGMANAGER_H
