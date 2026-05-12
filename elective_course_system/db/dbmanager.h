
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
    QSqlDatabase database();//数据库对象

private:

    DbManager() = default;

    ~DbManager() = default;

    DbManager(const DbManager&) = delete;

    DbManager& operator=(const DbManager&) = delete;

};
