

#include "dbmanager.h"
DbManager& DbManager::instance()
{
    static DbManager instance;
    return instance;
}
bool DbManager::connect()
{
    if (QSqlDatabase::contains("main_connection")) {
        QSqlDatabase db = QSqlDatabase::database("main_connection");
        if (db.isOpen()) {
            return true;
        }
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "main_connection");

    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("elective_course_system");
    db.setUserName("root");
    db.setPassword("12345678");

    if (!db.open()) {
        qDebug() << "数据库连接失败:";
        qDebug() << db.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功";
    return true;
}

QSqlDatabase DbManager::database()
{
    return QSqlDatabase::database("main_connection");
}

