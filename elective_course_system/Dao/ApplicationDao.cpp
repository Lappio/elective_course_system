#include "ApplicationDao.h"
#include "db/dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<ApplicationRecord> ApplicationDao::findAll(int statusFilter)
{
    QList<ApplicationRecord> list;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    if (statusFilter >= 0) {
        query.prepare("SELECT id, course_name, teacher_name, teacher_id, credit, max_capacity, reason, status, submit_time FROM course_applications WHERE status = :st ORDER BY submit_time DESC");
        query.bindValue(":st", statusFilter);
    } else {
        query.exec("SELECT id, course_name, teacher_name, teacher_id, credit, max_capacity, reason, status, submit_time FROM course_applications ORDER BY submit_time DESC");
    }
    if (query.lastError().isValid()) {
        qDebug() << "查询申请失败:" << query.lastError().text();
        return list;
    }
    while (query.next()) {
        ApplicationRecord r;
        r.id = query.value("id").toInt();
        r.courseName = query.value("course_name").toString();
        r.teacherName = query.value("teacher_name").toString();
        r.teacherId = query.value("teacher_id").toString();
        r.credit = query.value("credit").toFloat();
        r.maxCapacity = query.value("max_capacity").toInt();
        r.reason = query.value("reason").toString();
        r.status = query.value("status").toInt();
        r.submitTime = query.value("submit_time").toDateTime();
        list.append(r);
    }
    return list;
}

std::optional<ApplicationRecord> ApplicationDao::findById(int id)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("SELECT id, course_name, teacher_name, teacher_id, credit, max_capacity, reason, status, submit_time FROM course_applications WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec() || !query.next()) return std::nullopt;
    ApplicationRecord r;
    r.id = query.value("id").toInt();
    r.courseName = query.value("course_name").toString();
    r.teacherName = query.value("teacher_name").toString();
    r.teacherId = query.value("teacher_id").toString();
    r.credit = query.value("credit").toFloat();
    r.maxCapacity = query.value("max_capacity").toInt();
    r.reason = query.value("reason").toString();
    r.status = query.value("status").toInt();
    r.submitTime = query.value("submit_time").toDateTime();
    return r;
}

bool ApplicationDao::insert(const ApplicationRecord &app)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("INSERT INTO course_applications (course_name, teacher_name, teacher_id, credit, max_capacity, reason, status) VALUES (:name, :tname, :tid, :credit, :max, :reason, 0)");
    query.bindValue(":name", app.courseName);
    query.bindValue(":tname", app.teacherName);
    query.bindValue(":tid", app.teacherId);
    query.bindValue(":credit", app.credit);
    query.bindValue(":max", app.maxCapacity);
    query.bindValue(":reason", app.reason);
    if (!query.exec()) {
        qDebug() << "提交申请失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ApplicationDao::updateStatus(int id, int newStatus)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("UPDATE course_applications SET status = :st WHERE id = :id");
    query.bindValue(":st", newStatus);
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "更新申请状态失败:" << query.lastError().text();
        return false;
    }
    return true;
}
