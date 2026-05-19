#include "SelectionDao.h"
#include "db/dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<SelectionRecord> SelectionDao::findAll()
{
    QList<SelectionRecord> list;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.exec(
        "SELECT s.id, s.course_id, c.course_name, s.stu_id, u.user_name AS stu_name, s.select_time "
        "FROM selections s "
        "LEFT JOIN courses c ON s.course_id = c.course_id "
        "LEFT JOIN users u ON s.stu_id = u.id "
        "ORDER BY s.select_time DESC");
    while (query.next()) {
        SelectionRecord r;
        r.id = query.value("id").toInt();
        r.courseId = query.value("course_id").toString();
        r.courseName = query.value("course_name").toString();
        r.stuId = query.value("stu_id").toString();
        r.stuName = query.value("stu_name").toString();
        r.selectTime = query.value("select_time").toDateTime();
        list.append(r);
    }
    return list;
}

QList<SelectionRecord> SelectionDao::findByCourse(const QString &courseId)
{
    QList<SelectionRecord> list;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare(
        "SELECT s.id, s.course_id, c.course_name, s.stu_id, u.user_name AS stu_name, s.select_time "
        "FROM selections s "
        "LEFT JOIN courses c ON s.course_id = c.course_id "
        "LEFT JOIN users u ON s.stu_id = u.id "
        "WHERE s.course_id = :cid "
        "ORDER BY s.select_time DESC");
    query.bindValue(":cid", courseId);
    if (!query.exec()) return list;
    while (query.next()) {
        SelectionRecord r;
        r.id = query.value("id").toInt();
        r.courseId = query.value("course_id").toString();
        r.courseName = query.value("course_name").toString();
        r.stuId = query.value("stu_id").toString();
        r.stuName = query.value("stu_name").toString();
        r.selectTime = query.value("select_time").toDateTime();
        list.append(r);
    }
    return list;
}

QList<SelectionRecord> SelectionDao::findByStudent(const QString &stuId)
{
    QList<SelectionRecord> list;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare(
        "SELECT s.id, s.course_id, c.course_name, s.stu_id, u.user_name AS stu_name, s.select_time "
        "FROM selections s "
        "LEFT JOIN courses c ON s.course_id = c.course_id "
        "LEFT JOIN users u ON s.stu_id = u.id "
        "WHERE s.stu_id = :sid "
        "ORDER BY s.select_time DESC");
    query.bindValue(":sid", stuId);
    if (!query.exec()) return list;
    while (query.next()) {
        SelectionRecord r;
        r.id = query.value("id").toInt();
        r.courseId = query.value("course_id").toString();
        r.courseName = query.value("course_name").toString();
        r.stuId = query.value("stu_id").toString();
        r.stuName = query.value("stu_name").toString();
        r.selectTime = query.value("select_time").toDateTime();
        list.append(r);
    }
    return list;
}

bool SelectionDao::remove(int selectionId)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    // 先查出对应的课程ID
    query.prepare("SELECT course_id FROM selections WHERE id = :id");
    query.bindValue(":id", selectionId);
    if (!query.exec() || !query.next()) return false;
    QString courseId = query.value("course_id").toString();

    // 删除选课记录
    query.prepare("DELETE FROM selections WHERE id = :id");
    query.bindValue(":id", selectionId);
    if (!query.exec()) {
        qDebug() << "删除选课记录失败:" << query.lastError().text();
        return false;
    }
    // 更新课程已选人数
    query.prepare("SELECT COUNT(*) FROM selections WHERE course_id = :cid");
    query.bindValue(":cid", courseId);
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        query.prepare("UPDATE courses SET current_enrolled = :c WHERE course_id = :cid");
        query.bindValue(":c", count);
        query.bindValue(":cid", courseId);
        query.exec();
    }
    return true;
}

int SelectionDao::countByCourse(const QString &courseId)
{
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM selections WHERE course_id = :cid");
    query.bindValue(":cid", courseId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

QList<QPair<QString, int>> SelectionDao::enrollmentStats()
{
    QList<QPair<QString, int>> stats;
    QSqlDatabase &db = DbManager::instance().database();
    QSqlQuery query(db);
    query.exec(
        "SELECT c.course_name, c.current_enrolled "
        "FROM courses c WHERE c.status = 1 "
        "ORDER BY c.current_enrolled DESC");
    while (query.next()) {
        stats.append({query.value("course_name").toString(),
                      query.value("current_enrolled").toInt()});
    }
    return stats;
}
