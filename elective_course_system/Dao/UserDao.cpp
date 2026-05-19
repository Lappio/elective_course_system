#include"UserDao.h"
#include"db/dbmanager.h"
#include<qsqlquery.h>

std::optional<User> UserDao::findbyusername(const QString username){
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    // MySQL查询语句 - 使用正确的列名匹配数据库表结构
    query.prepare(
        "SELECT id, user_id, user_name, password_salt, password_hash, role "
        "FROM users "
        "WHERE user_name = :username"
        );
    query.bindValue(":username", username);
    if (!query.exec()) {
        qDebug() << "查询用户失败:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }
    User user;
    user.setpassword_Hash(query.value("password_hash").toString());
    user.setpassword_Salt(query.value("password_salt").toString());
    // role字段在数据库中是CHAR(1)，但这里转为int使用
    user.setrole(query.value("role").toInt());
    // 使用数据库自增id作为user的唯一标识（与selections表的stu_id关联一致）
    user.setuser_id(query.value("id").toString());
    user.setuser_name(username);
    return user;
}
