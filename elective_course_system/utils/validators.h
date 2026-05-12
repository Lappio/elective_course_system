#pragma once
#include<QString>
#include <QMessageBox>
#include <QDebug>
//对用户输入的用户名以及密码进行明文检验
class validator{
    public:
    static bool validate_pass(const QString password);
    static bool validate_user(const QString username);

};
