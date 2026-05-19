#pragma once
#include <QList>
#include <QString>
#include <optional>

// 学生信息数据结构
struct StudentInfo {
    int dbId;          // 数据库自增id
    QString userId;    // 学号/工号 (user_id列)
    QString userName;  // 姓名
    QString role;      // 角色 '1'=学生 '0'=管理员 '2'=教师
};

// 学生数据访问层 - 负责学生用户的增删改查
class StudentDao
{
public:
    // 查询所有学生（role='1'）
    static QList<StudentInfo> findAllStudents();
    // 根据数据库id查询
    static std::optional<StudentInfo> findById(int dbId);
    // 根据学号查询
    static std::optional<StudentInfo> findByUserId(const QString &userId);
    // 搜索学生（按姓名或学号）
    static QList<StudentInfo> search(const QString &keyword);
    // 新增学生，返回错误信息（空字符串表示成功）
    static QString insertStudent(const StudentInfo &stu, const QString &password);
    // 更新学生信息
    static bool updateStudent(const StudentInfo &stu);
    // 删除学生（同时删除选课记录）
    static bool removeStudent(int dbId);
    // 获取所有课程列表（供选课管理下拉框使用）
    static QStringList getAllCourseNames();
};
