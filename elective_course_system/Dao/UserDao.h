#pragma once
#include<QString>
#include"model/user.h"
class UserDao{
public:
    static std::optional<User> findbyusername(const QString username);

};