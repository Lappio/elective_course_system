

#include "dbmanager.h"
#include <QSqlQuery>

DbManager& DbManager::instance()
{
    static DbManager instance;
    return instance;
}

DbManager::~DbManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DbManager::connect()
{
    if (QSqlDatabase::contains("main_connection")) {
        m_db = QSqlDatabase::database("main_connection");
        if (m_db.isOpen()) {
            return true;
        }
    } else {
        m_db = QSqlDatabase::addDatabase("QMYSQL", "main_connection");
    }

    m_db.setHostName("127.0.0.1");
    m_db.setPort(3306);
    m_db.setDatabaseName("elective_course_system");
    m_db.setUserName("root");
    m_db.setPassword("12345678");

    if (!m_db.open()) {
        qDebug() << "数据库连接失败:";
        qDebug() << m_db.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功";
    createTables();
    return true;
}

void DbManager::createTables()
{
    QSqlQuery query(m_db);

    // 课程表
    query.exec(
        "CREATE TABLE IF NOT EXISTS courses ("
        "course_id VARCHAR(50) PRIMARY KEY,"
        "course_name VARCHAR(200) NOT NULL,"
        "teacher_name VARCHAR(100) DEFAULT '',"
        "credit FLOAT DEFAULT 0,"
        "max_capacity INT DEFAULT 0,"
        "current_enrolled INT DEFAULT 0,"
        "status INT DEFAULT 1"
        ")");

    // 选课记录表
    query.exec(
        "CREATE TABLE IF NOT EXISTS selections ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "course_id VARCHAR(50),"
        "stu_id VARCHAR(50),"
        "select_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")");

    // 课程申请表
    query.exec(
        "CREATE TABLE IF NOT EXISTS course_applications ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "course_name VARCHAR(200) NOT NULL,"
        "teacher_name VARCHAR(100) DEFAULT '',"
        "teacher_id VARCHAR(50) DEFAULT '',"
        "credit FLOAT DEFAULT 0,"
        "max_capacity INT DEFAULT 0,"
        "reason TEXT,"
        "status INT DEFAULT 0,"
        "submit_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")");

    qDebug() << "数据库表初始化完成";
}

QSqlDatabase& DbManager::database()
{
    return m_db;
}

