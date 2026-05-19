
#pragma once
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
//定义一个DbManager类用于管理数据库
class DbManager

{
public:

    static DbManager& instance();//单例模式
    bool connect();//是否连接成功
    QSqlDatabase& database();//数据库对象（返回引用，保证连接生命周期）
    void createTables();//自动创建数据库表

private:

    DbManager() = default;

    ~DbManager();

    DbManager(const DbManager&) = delete;

    DbManager& operator=(const DbManager&) = delete;

    QSqlDatabase m_db; // 持有数据库连接，确保不被提前销毁
};
