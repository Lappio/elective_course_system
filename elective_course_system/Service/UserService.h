#pragma once
#include<QString>

class UserService
{

public:
    static bool login(const QString username,const QString password);
    UserService();
    ~UserService();
};
