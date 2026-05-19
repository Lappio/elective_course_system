#pragma once
#include <QList>
#include <QString>
#include <QDateTime>

// 选课记录数据结构
struct SelectionRecord {
    int id;
    QString courseId;
    QString courseName;
    QString stuId;
    QString stuName;
    QDateTime selectTime;
};

// 选课数据访问层 - 负责选课记录的数据库操作
class SelectionDao
{
public:
    // 查询所有选课记录（含课程名和学生名）
    static QList<SelectionRecord> findAll();
    // 按课程ID筛选选课记录
    static QList<SelectionRecord> findByCourse(const QString &courseId);
    // 按学生ID筛选选课记录
    static QList<SelectionRecord> findByStudent(const QString &stuId);
    // 删除选课记录
    static bool remove(int selectionId);
    // 获取某课程当前选课人数
    static int countByCourse(const QString &courseId);
    // 获取各课程选课人数统计（用于图表）
    static QList<QPair<QString, int>> enrollmentStats();
};
