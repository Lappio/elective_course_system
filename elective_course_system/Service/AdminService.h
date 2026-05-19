#pragma once
#include <QList>
#include <QPair>
#include "Dao/SelectionDao.h"
#include "Dao/ApplicationDao.h"

// 管理员综合业务逻辑 - 选课管理、课程审核、统计
class AdminService
{
public:
    // ===== 选课管理 =====
    // 获取所有选课记录
    static QList<SelectionRecord> getAllSelections();
    // 按课程筛选选课记录
    static QList<SelectionRecord> getSelectionsByCourse(const QString &courseId);
    // 退选某条选课记录
    static bool removeSelection(int selectionId);
    // 获取选课统计数据（各课程选课人数）
    static QList<QPair<QString, int>> getEnrollmentStats();

    // ===== 课程审核 =====
    // 获取待审核申请列表
    static QList<ApplicationRecord> getPendingApplications();
    // 获取所有申请列表
    static QList<ApplicationRecord> getAllApplications();
    // 通过申请（审核通过后自动创建课程）
    static QString approveApplication(int applicationId);
    // 拒绝申请
    static bool rejectApplication(int applicationId);
};
