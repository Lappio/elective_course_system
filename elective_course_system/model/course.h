#pragma once
#include<QString>
class Course{
public:
    QString course_id;
    QString course_name;
    QString teacher_name;
    float credit;
    int max_capacity;
    int current_enrolled;
    int status = 1; // 1=正常开课, 0=已停课
};
