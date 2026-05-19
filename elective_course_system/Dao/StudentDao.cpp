#include "StudentDao.h"
#include "db/dbmanager.h"
#include "utils/passwordutils.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<StudentInfo> StudentDao::findAllStudents()
{
    QList<StudentInfo> list;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.exec("SELECT id, user_id, user_name, role FROM users WHERE role = '1' ORDER BY user_id");
    while (query.next()) {
        StudentInfo s;
        s.dbId = query.value("id").toInt();
        s.userId = query.value("user_id").toString();
        s.userName = query.value("user_name").toString();
        s.role = query.value("role").toString();
        list.append(s);
    }
    return list;
}

std::optional<StudentInfo> StudentDao::findById(int dbId)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("SELECT id, user_id, user_name, role FROM users WHERE id = :id");
    query.bindValue(":id", dbId);
    if (!query.exec() || !query.next()) return std::nullopt;
    StudentInfo s;
    s.dbId = query.value("id").toInt();
    s.userId = query.value("user_id").toString();
    s.userName = query.value("user_name").toString();
    s.role = query.value("role").toString();
    return s;
}

std::optional<StudentInfo> StudentDao::findByUserId(const QString &userId)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("SELECT id, user_id, user_name, role FROM users WHERE user_id = :uid");
    query.bindValue(":uid", userId);
    if (!query.exec() || !query.next()) return std::nullopt;
    StudentInfo s;
    s.dbId = query.value("id").toInt();
    s.userId = query.value("user_id").toString();
    s.userName = query.value("user_name").toString();
    s.role = query.value("role").toString();
    return s;
}

QList<StudentInfo> StudentDao::search(const QString &keyword)
{
    QList<StudentInfo> list;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("SELECT id, user_id, user_name, role FROM users WHERE role = '1' AND (user_name LIKE :kw OR user_id LIKE :kw2) ORDER BY user_id");
    QString pattern = "%" + keyword + "%";
    query.bindValue(":kw", pattern);
    query.bindValue(":kw2", pattern);
    if (!query.exec()) return list;
    while (query.next()) {
        StudentInfo s;
        s.dbId = query.value("id").toInt();
        s.userId = query.value("user_id").toString();
        s.userName = query.value("user_name").toString();
        s.role = query.value("role").toString();
        list.append(s);
    }
    return list;
}

QString StudentDao::insertStudent(const StudentInfo &stu, const QString &password)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);

    // 检查学号是否已存在
    if(stu.userId.length()!=8){
        return "请输入正确的学号!";
    }
    query.prepare("SELECT COUNT(*) FROM users WHERE user_id = :uid");
    query.bindValue(":uid", stu.userId);
    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        return "学号已存在！";
    }


    // 生成盐值和密码哈希
    QString salt = PasswordUtils::makeSalt();
    QString hash = PasswordUtils::hashPassword(password, salt);

    query.prepare("INSERT INTO users (user_id, user_name, password_salt, password_hash, role) VALUES (:uid, :uname, :salt, :hash, '1')");
    query.bindValue(":uid", stu.userId);
    query.bindValue(":uname", stu.userName);
    query.bindValue(":salt", salt);
    query.bindValue(":hash", hash);
    if (!query.exec()) {
        qDebug() << "新增学生失败:" << query.lastError().text();
        return "数据库写入失败: " + query.lastError().text();
    }
    query.prepare("INSERT INTO student (stu_id,name) VALUES(:uid, :uname)");
    query.bindValue(":uid",stu.userId);
    query.bindValue(":uname",stu.userName);
    if(!query.exec()){
        qDebug() << "新增学生失败:" << query.lastError().text();
        return "数据库写入失败: " + query.lastError().text();
    }
    return QString(); // 成功
}

bool StudentDao::updateStudent(const StudentInfo &stu)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);

    // 检查新学号是否与其他记录冲突
    query.prepare("SELECT COUNT(*) FROM users WHERE user_id = :uid AND id != :id");
    query.bindValue(":uid", stu.userId);
    query.bindValue(":id", stu.dbId);
    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        qDebug() << "更新学生失败: 学号已存在";
        return false;
    }

    query.prepare("UPDATE users SET user_id = :uid, user_name = :uname WHERE id = :id");
    query.bindValue(":uid", stu.userId);
    query.bindValue(":uname", stu.userName);
    query.bindValue(":id", stu.dbId);
    if (!query.exec()) {
        qDebug() << "更新学生失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool StudentDao::removeStudent(int dbId)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);

    // 先删除该学生的所有选课记录
    query.prepare("DELETE FROM selections WHERE stu_id = :id");
    query.bindValue(":id", dbId);
    query.exec();

    // 再删除学生用户
    query.prepare("DELETE FROM users WHERE id = :id AND role = '1'");
    query.bindValue(":id", dbId);
    if (!query.exec()) {
        qDebug() << "删除学生失败:" << query.lastError().text();
        return false;
    }
    return true;
}

QStringList StudentDao::getAllCourseNames()
{
    QStringList names;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.exec("SELECT course_id, course_name FROM courses WHERE status = 1 ORDER BY course_id");
    while (query.next()) {
        names.append(query.value("course_id").toString() + " - " + query.value("course_name").toString());
    }
    return names;
}
