#pragma once
#include <QList>
#include <QString>
#include <QDateTime>

// 课程申请数据结构
struct ApplicationRecord {
    int id;
    QString courseName;
    QString teacherName;
    QString teacherId;
    float credit;
    int maxCapacity;
    QString reason;
    int status;      // 0=待审核, 1=已通过, 2=已拒绝
    QDateTime submitTime;
};

// 课程申请数据访问层 - 负责教师提交的课程申请的数据库操作
class ApplicationDao
{
public:
    // 查询所有申请（可按状态筛选）
    static QList<ApplicationRecord> findAll(int statusFilter = -1);
    // 根据ID查询
    static std::optional<ApplicationRecord> findById(int id);
    // 新增课程申请
    static bool insert(const ApplicationRecord &app);
    // 更新申请状态（通过/拒绝）
    static bool updateStatus(int id, int newStatus);
};
