#pragma once
#include <QString>
#include <QList>
#include "model/course.h"

// 课程业务逻辑层 - 负责课程相关校验和操作
class CourseService
{
public:
    // 获取所有课程
    static QList<Course> getAllCourses();
    // 搜索课程
    static QList<Course> searchCourses(const QString &keyword);
    // 新增课程（含校验），返回错误信息，空字符串表示成功
    static QString addCourse(const Course &course);
    // 更新课程（含校验），返回错误信息，空字符串表示成功
    static QString updateCourse(const Course &course);
    // 删除课程
    static bool deleteCourse(const QString &courseId);
    // 校验课程数据
    static QString validateCourse(const Course &course);
};
