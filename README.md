### 基于C++,Qt,MySQL的选修课程管理系统

## 1.1 组织数据
本系统以 user 表实现统一登录认证，通过 role 字段区分管理员和学生身份；
student 表和 admin 表分别保存两类用户的详细信息；
course 表保存选修课程的基本信息；
selection 表作为学生与课程之间的中间表，用于表示学生选课关系。
其中 student 与 course 是多对多关系，系统通过 selection 表将其拆分为两个一对多关系，从而方便完成选课、退课、查询和统计等功能。
# 1.1.1初步的建立
首先建立elective_course_system数据库
然后考虑数据库中需要建立哪些表格
首先是user表,到时候的登陆窗口对接此表,user表根据role字段来区分登陆用户的身份,然后转到studen表和admin表。关于登陆密码这一块，我考虑用正则表达式进行一个校验，现在暂时还不会，放入todo里。
考虑新增一个teacher身份，这样能做更多的功能，暂时搁置。
```sql
show tables ;
show databases ;
create database elective_course_system;
use elective_course_system;
create table student(
    stu_id char(8) comment '学号是定长8位的字符串',
    name varchar(15) NOT NULL comment '学号不能为空最长15个字符',
    gender char(1) NOT NULL comment '性别必须存在并且只有1个字符'
);
create table users(
    user_id char(8) comment '用户id也为8位',
    user_name varchar(15) NOT NULL comment '不为空的至多15位字符',
    user_password varchar(30) NOT NULL comment '限制长度和非空,需要正则表达式来进一步限制密码',
    role char(1) NOT NULL comment '此字段用来判断user的身份'
);
create table admin(
    admin_id varchar(3) comment '管理员id',
    admin_name varchar(15) comment '不为空的至多15位字符'
)
```
下面要做的是将users表分别和users表和admin表用键来建立关联
前面的约束条件没有写好
重新改一下
users表:
users里面加一个自增的id作为主键PRIMARY KEY,AUTO_INCREMENT,
user_id 加一个非空且唯一的的约束关键字
password用check加一个正则表达式来约束
role设置一个默认权限(学生)
users_name不动
student表同理
加一个自增的id作为主键
stu_id唯一且不为空
gender,name不变
admin也一样
student和admin里需要加一个user_id外键关联到users表
也就是说users表是父表,而student和admin表都是子表 
```sql
#对users表的调整
alter table users add id int auto_increment primary key comment '加一个自增的id';
alter table users add UNIQUE (user_id)comment '非空且唯一';
alter table users add CHECK (user_password REGEXP '^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9]).{8,}$' );
#对student表和admin表的调整
alter table student add id int auto_increment primary key comment '加一个自增的id';
alter table student add UNIQUE (stu_id);
alter table student add column user_id char(8) UNIQUE NOT NULL ;
alter table student add constraint fk_user_id foreign key (user_id) references users(user_id);
-- 对admin表的操作
alter table admin add UNIQUE (admin_id);
alter table admin add column user_id char(8) UNIQUE NOT NULL ;
alter table admin add id int auto_increment primary key comment '加一个自增的id';
alter table admin add constraint fk_admin_id foreign key (user_id) references users(user_id);
desc student;
insert into users (id,user_name,user_password,user_id,role) VALUES (1,'乔一腾',
                                                                    'Qyt20051012',
                                                                    '20244489','0');
insert into student (id,stu_id, name, gender,user_id) VALUES (1,'20244489',
                                                              '乔一腾','男','20244489');
insert into users (id,user_name,user_password,user_id,role) VALUES (2,'admin',
                                                                    'Qq123456',
                                                                    '00000001','1');
insert into admin (id, admin_id,admin_name, user_id) VALUES (1,'001',
                                                              'admin','00000001');
```
这样一来用户表和管理员表以及学生表的关系就基本完成了
# 1.1.2 学生表与选课表的关联
学生表要与选课表进行关联，选课表要与课程清单表关联
也就是说，需要再建立两个表
用stu_id关联selection表与course表
用course_id关联course表与selection表

建立selection表时出现的错误
```sql
create table selection(
    id int auto_increment unique PRIMARY KEY ,
    stu_id char(8) unique not null,  -- 这里有问题！
    course_id char(7) unique not null -- 这里也有问题！
);
```
存在逻辑漏洞
一名学生应该能选多门课，所以不能给stu_id加上unique约束
应该改为
```sql
create table selection(
    id int auto_increment PRIMARY KEY,
    stu_id char(8) not null comment '外键，关联学生',
    course_id char(7) not null comment '外键，关联课程',
    -- 联合唯一约束：同一个学生不能重复选同一门课
    CONSTRAINT uk_stu_course UNIQUE (stu_id, course_id),
    -- 添加外键关联
    CONSTRAINT fk_selection_stu FOREIGN KEY (stu_id) REFERENCES student(stu_id) ON DELETE CASCADE,
    CONSTRAINT fk_selection_course FOREIGN KEY (course_id) REFERENCES course(course_id) ON DELETE CASCADE
);
```
做一个联合唯一约束也就是说单列可以重复但是不能两列都重复，同时添加上外键关联

数据库的构建第一版先这样
下面进行用qt项目来连接数据库(Gemini 3.1pro)

第一步是修改cmakelists 导入sql模块
```cmake
# 找到类似这样的一行，加上 Sql
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
# 找到类似这样的一行，加上 Qt6::Sql
target_link_libraries(elective_course_system PRIVATE Qt6::Widgets Qt6::Sql)
```
下面修改mainwindow的代码
```cpp
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
```
在mainwindow.cpp里引入头文件
修改代码
```cpp
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");   // 创建数据库对象
    db.setDatabaseName("elective_course_system");
    db.setHostName("127.0.0.1");
    db.setPort(3306);  //这是MySQL的默认端口
    db.setUserName("root") ;  //以root权限访问数据库
    db.setPassword("12345678"); //这边需要填自己的密码，我的电脑上设置的是12345678
    if(db.open()){
        QMessageBox::information(this,"数据库连接成功!","Niceeeeeeeee!!!!!");
    }
    else {
        // 如果连接失败，打印错误信息并弹窗
        qDebug() << "数据库连接失败: " << db.lastError().text();
        QMessageBox::critical(this, "连接失败", "无法连接数据库：\n" + db.lastError().text());
    }
```
第一次连接的时候遇到了一个奇怪的错误
qt.sql.qsqldatabase: QSqlDatabase: can not load requested driver 'QMYSQL', available drivers: QSQLITE QODBC QPSQL QMIMER
数据库连接失败:  "Driver not loaded Driver not loaded"
目前因为 开源版权协议（GPL）的冲突，Qt 官方在 macOS 版本里，不再直接提供编译好的 MySQL 驱动文件（libqsqlmysql.dylib）。
解决方法
使用homebrew安装一个支持sql的qt




组织此项目的文件结构
```plain text
elective_course_system/
├── CMakeLists.txt
├── main.cpp
│
├── ui/                         # 界面层：窗口、控件、信号槽
│   ├── login/
│   │   ├── loginwindow.h
│   │   ├── loginwindow.cpp
│   │   └── loginwindow.ui
│   ├── student/
│   │   ├── studentwindow.h
│   │   ├── studentwindow.cpp
│   │   └── studentwindow.ui
│   └── admin/
│       ├── adminwindow.h
│       ├── adminwindow.cpp
│       └── adminwindow.ui
│
├── db/                         # 数据库基础设施
│   ├── dbmanager.h
│   └── dbmanager.cpp
│
├── dao/                        # 数据访问层：专门写 SQL
│   ├── userdao.h
│   ├── userdao.cpp
│   ├── coursedao.h
│   ├── coursedao.cpp
│   ├── selectiondao.h
│   └── selectiondao.cpp
│
├── model/                      # 实体模型：User、Course、Selection 等
│   ├── user.h
│   ├── course.h
│   └── selection.h
│
├── service/                    # 业务逻辑层：权限、选课规则、事务
│   ├── userservice.h
│   ├── userservice.cpp
│   ├── courseservice.h
│   └── courseservice.cpp
│
├── utils/                      # 通用工具
│   ├── passwordutils.h
│   ├── passwordutils.cpp
│   ├── validators.h
│   └── validators.cpp
│
├── resources/
│   ├── app_resources.qrc
│   ├── images/
│   └── styles/
│       └── app.qss
│
└── docs/                       # 课设文档、数据库表结构、ER 图
    ├── database.sql
    ├── er_diagram.png
    └── README.md
    ```
model文件夹定义Course,Selection,User类,这三个类仅用于接收数据,并不会直接写SQL
我的开工顺序
1. docs/database.sql
2. model/user.h
3. model/course.h
4. model/selection.h
5. db/dbmanager.h / dbmanager.cpp
6. dao/userdao.h / userdao.cpp
7. utils/passwordutils.h / passwordutils.cpp
8. service/userservice.h / userservice.cpp
9. ui/login/loginwindow
10. dao/coursedao.h / coursedao.cpp
11. dao/selectiondao.h / selectiondao.cpp
12. service/courseservice.h / courseservice.cpp
13. ui/student/studentwindow
14. ui/admin/adminwindow
15. resources/styles/app.qss
16. docs/README.md
为了调试代码，新建一个tests文件夹
里面会写需要调试文件的入口
然后进行调试
