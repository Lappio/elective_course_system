#pragma once
#include <QList>
#include <optional>
#include "model/course.h"

// 课程数据访问层 - 负责课程的数据库增删改查操作
class CourseDao
{
public:
    // 查询所有课程
    static QList<Course> findAll();
    // 根据课程ID查询
    static std::optional<Course> findById(const QString &courseId);
    // 按课程名或教师名模糊搜索
    static QList<Course> search(const QString &keyword);
    // 新增课程
    static bool insert(const Course &course);
    // 更新课程信息
    static bool update(const Course &course);
    // 删除课程
    static bool remove(const QString &courseId);
    // 更新课程已选人数
    static bool updateEnrolledCount(const QString &courseId, int count);
};
