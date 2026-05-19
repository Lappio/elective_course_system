#include "UserService.h"
#include"utils/passwordutils.h"
#include"utils/validators.h"
#include"model/user.h"
#include"Dao/UserDao.h"
#include"adminwindow.h"
#include<QMessageBox>
bool UserService::login(const QString username,const QString password){
    if(!validator::validate_user(username)){
        QMessageBox::warning(nullptr,"⚠️警告","用户名不能为空!");
        return false;
    }
    else if(!validator::validate_pass(password)){
        QMessageBox::warning(nullptr,"⚠️警告","密码至少8位且包含大小写字母!");
        return false;
    }
    auto Userobj =  UserDao::findbyusername(username);
    if(!Userobj.has_value()){
        QMessageBox::warning(nullptr,"⚠️警告","用户不存在!");
        return false;
    }
    User user = Userobj.value();
    if(PasswordUtils::verifyPassword(password,
                                      user.getpasswordSalt(),
                                      user.getpasswordHash())){
        // 根据角色跳转到不同窗口
        if(user.getrole() == 0){
            // 管理员角色 -> 管理员后台
            AdminWindow *adminWin = new AdminWindow();
            adminWin->show();
        }else{
            // 学生角色 -> 学生选课窗口（待实现）
            QMessageBox::information(nullptr,"登录成功","学生端功能开发中，敬请期待!");
        }
        return true;
    }
    else{
        QMessageBox::warning(nullptr,"⚠️警告","密码错误!");
        return false;
    }

}