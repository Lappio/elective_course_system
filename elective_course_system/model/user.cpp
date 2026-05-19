//我是getter和setter函数
#include"user.h"
void User::setuser_name(const QString name){
    user_name = name;
}
void User::setpassword_Hash(const QString Hash)
{
    passwordHash = Hash;
}
void User::setpassword_Salt(const QString Salt){
    passwordSalt = Salt;
}
void User::setrole(int r){
    role = r;
}
void User::setuser_id(const QString id){
    user_id = id;
}
QString User::getuser_name(){
    return user_name;
}
QString User::getpasswordHash(){
    return passwordHash;
}
QString User::getpasswordSalt(){
    return passwordSalt;
}
QString User::getuser_id(){
    return user_id;
}
int User::getrole(){
    return role;
}
