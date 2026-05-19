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
 选修课管理系统 - 管理员界面实现计划                                                                                    
                                                                               
 Context

 当前项目是一个基于 Qt6 + C++ + MySQL
 的教务选课系统，已有登录窗口和基础架构（MVC分层：Model/DAO/Service/UI）。管理员界面（AdminWindow）目前只是一个空的
 QWidget 占位，需要实现完整的后台管理功能，包括课程CRUD、选课管理、数据可视化、课程申请审核。

 需要修改/新增的文件

 新增文件（共9个）

 ┌────────────────────────────────┬────────────────────────────────────────────┐
 │              文件              │                    用途                    │
 ├────────────────────────────────┼────────────────────────────────────────────┤
 │ Dao/CourseDao.h + .cpp         │ 课程的增删改查数据库操作                   │
 ├────────────────────────────────┼────────────────────────────────────────────┤
 │ Dao/SelectionDao.h + .cpp      │ 选课记录的数据库操作                       │
 ├────────────────────────────────┼────────────────────────────────────────────┤
 │ Dao/ApplicationDao.h + .cpp    │ 教师课程申请的数据库操作                   │
 ├────────────────────────────────┼────────────────────────────────────────────┤
 │ Service/CourseService.h + .cpp │ 课程业务逻辑（含校验）                     │
 ├────────────────────────────────┼────────────────────────────────────────────┤
 │ Service/AdminService.h + .cpp  │ 管理员综合业务逻辑（选课管理、审核、统计） │
 ├────────────────────────────────┼────────────────────────────────────────────┤
 │ expandable_features.md         │ 可扩展功能清单文档                         │
 └────────────────────────────────┴────────────────────────────────────────────┘

 修改文件（共5个）

 ┌─────────────────────────┬────────────────────────────────────────────┐
 │          文件           │                  变更内容                  │
 ├─────────────────────────┼────────────────────────────────────────────┤
 │ adminwindow.h           │ 改为 QMainWindow，添加所有槽函数和成员     │
 ├─────────────────────────┼────────────────────────────────────────────┤
 │ adminwindow.cpp         │ 完整的管理员界面逻辑实现                   │
 ├─────────────────────────┼────────────────────────────────────────────┤
 │ adminwindow.ui          │ 完整的 Tab 页 UI 布局                      │
 ├─────────────────────────┼────────────────────────────────────────────┤
 │ CMakeLists.txt          │ 添加新源文件和 Qt Charts 模块              │
 ├─────────────────────────┼────────────────────────────────────────────┤
 │ Service/UserService.cpp │ 修复 AdminWindow 悬垂指针 bug + 按角色跳转 │
 └─────────────────────────┴────────────────────────────────────────────┘

 实现架构

 沿用现有 MVC 分层模式：
 Model (user.h, course.h, selection.h)
   -> Dao (CourseDao, SelectionDao, ApplicationDao)
     -> Service (CourseService, AdminService)
       -> UI (AdminWindow)

 数据库表设计

 需要在 MySQL elective_course_system 数据库中创建以下表：

 -- 课程表
 CREATE TABLE IF NOT EXISTS courses (
     course_id VARCHAR(50) PRIMARY KEY,
     course_name VARCHAR(200) NOT NULL,
     teacher_name VARCHAR(100) DEFAULT '',
     credit FLOAT DEFAULT 0,
     max_capacity INT DEFAULT 0,
     current_enrolled INT DEFAULT 0,
     status INT DEFAULT 1
 );

 -- 选课记录表
 CREATE TABLE IF NOT EXISTS selections (
     id INT AUTO_INCREMENT PRIMARY KEY,
     course_id VARCHAR(50),
     stu_id VARCHAR(50),
     select_time DATETIME DEFAULT CURRENT_TIMESTAMP
 );

 -- 教师课程申请表
 CREATE TABLE IF NOT EXISTS course_applications (
     id INT AUTO_INCREMENT PRIMARY KEY,
     course_name VARCHAR(200) NOT NULL,
     teacher_name VARCHAR(100) DEFAULT '',
     teacher_id VARCHAR(50) DEFAULT '',
     credit FLOAT DEFAULT 0,
     max_capacity INT DEFAULT 0,
     reason TEXT,
     status INT DEFAULT 0
 );

 表创建通过 DbManager 在应用启动时自动执行，无需手动 SQL。

 AdminWindow UI 设计

 整体布局

 - 继承 QMainWindow（而非当前 QWidget），窗口标题"教务选课系统 - 管理员后台"
 - 使用 QTabWidget 分四个标签页：
   a. 课程管理 - 课程增删改查
   b. 选课管理 - 查看/删除学生选课记录
   c. 数据可视化 - 选课情况图表
   d. 课程审核 - 审核教师提交的课程申请

 Tab 1: 课程管理

 - 顶部：搜索栏（按课程名/教师名筛选）+ 新增课程按钮
 - 中间：QTableWidget 展示所有课程（课程ID、名称、教师、学分、容量、已选人数）
 - 底部：编辑选中课程 / 删除选中课程 按钮
 - 新增/编辑使用 QDialog 弹窗

 Tab 2: 选课管理

 - 顶部：筛选下拉框（按课程筛选）
 - 中间：QTableWidget 展示选课记录（学生ID、学生名、课程名、选课时间）
 - 操作：退选按钮（删除选课记录，同时更新课程的 current_enrolled）

 Tab 3: 数据可视化

 - 使用 Qt Charts (QChartView) 展示：
   - 柱状图：各课程选课人数对比
   - 饼图：课程选课率分布（已选/剩余）
 - 顶部刷新按钮

 Tab 4: 课程审核

 - 左侧：待审核申请列表（QTableWidget）
 - 右侧：申请详情展示 + 通过/拒绝按钮

 关键技术点

 1. AdminWindow 改为 QMainWindow：支持菜单栏、状态栏、更灵活布局
 2. 图表使用 Qt Charts：需要在 CMakeLists.txt 添加 Qt::Charts，install 时需 brew install qt6-charts（如未安装）
 3. 修复 UserService.cpp 中的严重 bug：第27行 AdminWindow *a; a->show(); 是未初始化指针，会导致 crash
 4. UserService 按角色跳转：role=0 管理员 -> AdminWindow，role=1 学生 -> 学生窗口（暂留接口）

 验证方式

 1. 确保 MySQL 服务运行，数据库 elective_course_system 存在
 2. 构建项目：cd build && cmake .. && make
 3. 启动应用，登录管理员账号（需预先在数据库插入管理员用户）
 4. 测试四个标签页的功能是否正常
 5. 测试课程 CRUD 流程
 6. 测试选课管理和退选逻辑
 7. 确认图表正常渲染
 8. 测试课程申请审核流程