#include "AdminService.h"
#include "Dao/CourseDao.h"
#include "model/course.h"
#include <QUuid>
#include <QDebug>

// ===== 选课管理 =====

QList<SelectionRecord> AdminService::getAllSelections()
{
    return SelectionDao::findAll();
}

QList<SelectionRecord> AdminService::getSelectionsByCourse(const QString &courseId)
{
    return SelectionDao::findByCourse(courseId);
}

bool AdminService::removeSelection(int selectionId)
{
    return SelectionDao::remove(selectionId);
}

QList<QPair<QString, int>> AdminService::getEnrollmentStats()
{
    return SelectionDao::enrollmentStats();
}

// ===== 课程审核 =====

QList<ApplicationRecord> AdminService::getPendingApplications()
{
    return ApplicationDao::findAll(0); // status=0 待审核
}

QList<ApplicationRecord> AdminService::getAllApplications()
{
    return ApplicationDao::findAll();
}

QString AdminService::approveApplication(int applicationId)
{
    auto appOpt = ApplicationDao::findById(applicationId);
    if (!appOpt.has_value()) {
        return "申请记录不存在！";
    }

    ApplicationRecord app = appOpt.value();
    if (app.status != 0) {
        return "该申请已被处理，无法重复审核！";
    }

    // 自动生成课程编号: C + 时间戳
    QString courseId = "C" + QString::number(QDateTime::currentSecsSinceEpoch());

    // 创建课程
    Course course;
    course.course_id = courseId;
    course.course_name = app.courseName;
    course.teacher_name = app.teacherName;
    course.credit = app.credit;
    course.max_capacity = app.maxCapacity;
    course.current_enrolled = 0;
    course.status = 1;

    if (!CourseDao::insert(course)) {
        return "创建课程失败，请检查数据库！";
    }

    // 更新申请状态为"已通过"
    ApplicationDao::updateStatus(applicationId, 1);
    return QString(); // 成功
}

bool AdminService::rejectApplication(int applicationId)
{
    return ApplicationDao::updateStatus(applicationId, 2);
}
