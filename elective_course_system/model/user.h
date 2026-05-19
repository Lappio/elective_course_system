#pragma once
#include<QString>


class User{
private:
    QString user_name;
    QString passwordSalt;
    int role;
    QString user_id;
    QString passwordHash;
public:
    void setuser_name(const QString name);
    void setpassword_Hash(const QString Hash);
    void setpassword_Salt(const QString Salt);
    void setrole(int r);
    void setuser_id(const QString id);
    QString getuser_name();
    QString getpasswordSalt();
    QString getuser_id();
    int getrole();
    QString getpasswordHash();
};


class Admin:public User{

};
class Student:public User{

};