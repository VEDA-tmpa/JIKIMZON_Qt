#include "eventlogmanager.h"

EventLogManager::EventLogManager(const QString &dbPath, QObject *parent)
    : QObject(parent)
{
    mDB = QSqlDatabase::addDatabase("QSQLITE");
    mDB.setDatabaseName(dbPath);

    if (!mDB.open())
    {
        qDebug() << "Error: Unable to open database." << mDB.lastError().text();
        return;
    }

    // 테이블 생성
    createTable();
}

EventLogManager::~EventLogManager() {
    if (mDB.isOpen())
    {
        mDB.close();
    }
}

void EventLogManager::createTable()
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS event_logs ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "frame_id INTEGER, "
               "timestamp TEXT, "
               "object_class TEXT, "
               "x INTEGER, "
               "y INTEGER, "
               "width INTEGER, "
               "height INTEGER);");

    if (query.lastError().isValid())
    {
        qDebug() << "Failed to create table:" << query.lastError().text();
    }
    else
    {
        qDebug() << "Table created successfully.";
    }
}


void EventLogManager::saveEventLog(QJsonObject &obj)
{
    if (obj.isEmpty())
    {
        qDebug() << "Empty json object";
        return;
    }

    int frameId = obj["frameId"].toInt();
    QString timestamp = obj["timestamp"].toString();

    QJsonArray objectArray = obj["object"].toArray();
    for (const QJsonValue &value : objectArray)
    {
        QJsonObject objData = value.toObject();
        QString className = objData["className"].toString();
        int x = objData["x"].toInt();
        int y = objData["y"].toInt();
        int width = objData["width"].toInt();
        int height = objData["height"].toInt();

        QSqlQuery query;
        query.prepare("INSERT INTO event_logs (frame_id, timestamp, object_class, x, y, width, height) "
                      "VALUES (:frame_id, :timestamp, :object_class, :x, :y, :width, :height)");
        query.bindValue(":frame_id", frameId);
        query.bindValue(":timestamp", timestamp);
        query.bindValue(":object_class", className);
        query.bindValue(":x", x);
        query.bindValue(":y", y);
        query.bindValue(":width", width);
        query.bindValue(":height", height);

        if (!query.exec())
        {
            qDebug() << "Failed to insert event log:" << query.lastError().text();
        }
    }
}
