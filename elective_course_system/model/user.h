#pragma once
#include<QString>
class User{
private:
    QString user_name;
    QString user_password;
    QString role;
    QString user_id;
    QString phone_number;
};
class Admin:public User{

};
class Student:public User{

};