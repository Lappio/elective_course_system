#include "CourseService.h"
#include "Dao/CourseDao.h"
#include <QMessageBox>

QList<Course> CourseService::getAllCourses()
{
    return CourseDao::findAll();
}

QList<Course> CourseService::searchCourses(const QString &keyword)
{
    if (keyword.trimmed().isEmpty()) {
        return CourseDao::findAll();
    }
    return CourseDao::search(keyword.trimmed());
}

QString CourseService::validateCourse(const Course &course)
{
    if (course.course_id.trimmed().isEmpty()) {
        return "课程编号不能为空！";
    }
    if (course.course_name.trimmed().isEmpty()) {
        return "课程名称不能为空！";
    }
    if (course.credit < 0 || course.credit > 20) {
        return "学分范围应在0-20之间！";
    }
    if (course.max_capacity <= 0 || course.max_capacity > 500) {
        return "课程容量应在1-500之间！";
    }
    return QString(); // 空字符串表示校验通过
}

QString CourseService::addCourse(const Course &course)
{
    QString err = validateCourse(course);
    if (!err.isEmpty()) return err;

    // 检查课程ID是否已存在
    auto existing = CourseDao::findById(course.course_id);
    if (existing.has_value()) {
        return "课程编号已存在！";
    }

    if (!CourseDao::insert(course)) {
        return "数据库操作失败，请重试！";
    }
    return QString();
}

QString CourseService::updateCourse(const Course &course)
{
    QString err = validateCourse(course);
    if (!err.isEmpty()) return err;

    if (!CourseDao::update(course)) {
        return "数据库操作失败，请重试！";
    }
    return QString();
}

bool CourseService::deleteCourse(const QString &courseId)
{
    return CourseDao::remove(courseId);
}
