#include "CourseDao.h"
#include "db/dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<Course> CourseDao::findAll()
{
    QList<Course> list;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.exec("SELECT course_id, course_name, teacher_name, credit, max_capacity, current_enrolled, status FROM courses ORDER BY course_id");
    while (query.next()) {
        Course c;
        c.course_id = query.value("course_id").toString();
        c.course_name = query.value("course_name").toString();
        c.teacher_name = query.value("teacher_name").toString();
        c.credit = query.value("credit").toFloat();
        c.max_capacity = query.value("max_capacity").toInt();
        c.current_enrolled = query.value("current_enrolled").toInt();
        c.status = query.value("status").toInt();
        list.append(c);
    }
    return list;
}

std::optional<Course> CourseDao::findById(const QString &courseId)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("SELECT course_id, course_name, teacher_name, credit, max_capacity, current_enrolled, status FROM courses WHERE course_id = :id");
    query.bindValue(":id", courseId);
    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }
    Course c;
    c.course_id = query.value("course_id").toString();
    c.course_name = query.value("course_name").toString();
    c.teacher_name = query.value("teacher_name").toString();
    c.credit = query.value("credit").toFloat();
    c.max_capacity = query.value("max_capacity").toInt();
    c.current_enrolled = query.value("current_enrolled").toInt();
    c.status = query.value("status").toInt();
    return c;
}

QList<Course> CourseDao::search(const QString &keyword)
{
    QList<Course> list;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("SELECT course_id, course_name, teacher_name, credit, max_capacity, current_enrolled, status FROM courses WHERE course_name LIKE :kw OR teacher_name LIKE :kw2 ORDER BY course_id");
    QString pattern = "%" + keyword + "%";
    query.bindValue(":kw", pattern);
    query.bindValue(":kw2", pattern);
    if (!query.exec()) {
        qDebug() << "搜索课程失败:" << query.lastError().text();
        return list;
    }
    while (query.next()) {
        Course c;
        c.course_id = query.value("course_id").toString();
        c.course_name = query.value("course_name").toString();
        c.teacher_name = query.value("teacher_name").toString();
        c.credit = query.value("credit").toFloat();
        c.max_capacity = query.value("max_capacity").toInt();
        c.current_enrolled = query.value("current_enrolled").toInt();
        c.status = query.value("status").toInt();
        list.append(c);
    }
    return list;
}

bool CourseDao::insert(const Course &course)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("INSERT INTO courses (course_id, course_name, teacher_name, credit, max_capacity, current_enrolled, status) VALUES (:id, :name, :teacher, :credit, :max, 0, 1)");
    query.bindValue(":id", course.course_id);
    query.bindValue(":name", course.course_name);
    query.bindValue(":teacher", course.teacher_name);
    query.bindValue(":credit", course.credit);
    query.bindValue(":max", course.max_capacity);
    if (!query.exec()) {
        qDebug() << "新增课程失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool CourseDao::update(const Course &course)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("UPDATE courses SET course_name = :name, teacher_name = :teacher, credit = :credit, max_capacity = :max, current_enrolled = :enrolled, status = :status WHERE course_id = :id");
    query.bindValue(":name", course.course_name);
    query.bindValue(":teacher", course.teacher_name);
    query.bindValue(":credit", course.credit);
    query.bindValue(":max", course.max_capacity);
    query.bindValue(":enrolled", course.current_enrolled);
    query.bindValue(":status", course.status);
    query.bindValue(":id", course.course_id);
    if (!query.exec()) {
        qDebug() << "更新课程失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool CourseDao::remove(const QString &courseId)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    // 先删除该课程的所有选课记录
    query.prepare("DELETE FROM selections WHERE course_id = :id");
    query.bindValue(":id", courseId);
    query.exec();
    // 再删除课程本身
    query.prepare("DELETE FROM courses WHERE course_id = :id");
    query.bindValue(":id", courseId);
    if (!query.exec()) {
        qDebug() << "删除课程失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool CourseDao::updateEnrolledCount(const QString &courseId, int count)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("UPDATE courses SET current_enrolled = :count WHERE course_id = :id");
    query.bindValue(":count", count);
    query.bindValue(":id", courseId);
    if (!query.exec()) {
        qDebug() << "更新选课人数失败:" << query.lastError().text();
        return false;
    }
    return true;
}
