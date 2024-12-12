#ifndef EVENTLOGMANAGER_H
#define EVENTLOGMANAGER_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QJsonObject>

class EventLogManager : public QObject
{
    Q_OBJECT
public:
    explicit EventLogManager(const QString &dbPath, QObject *parent = nullptr);
    ~EventLogManager();

    void createTable();
    void clearTable();
    void saveEventLog(QJsonObject& obj);

signals:

private:
    QSqlDatabase db;
};

#endif // EVENTLOGMANAGER_H
