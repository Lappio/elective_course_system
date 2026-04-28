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

