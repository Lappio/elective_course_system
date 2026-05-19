#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "Service/CourseService.h"
#include "Service/AdminService.h"
#include "Dao/CourseDao.h"
#include "Dao/StudentDao.h"
#include "model/course.h"
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QPainter>
#include <QHeaderView>
#include <QDateTime>

// =====================================================================
// ChartWidget 实现 - 自定义图表绘制控件
// =====================================================================

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent)
{
    // 初始化颜色方案 - 柔和的配色
    colors = {
        QColor("#0984e3"), QColor("#00b894"), QColor("#fdcb6e"),
        QColor("#e17055"), QColor("#6c5ce7"), QColor("#fd79a8"),
        QColor("#00cec9"), QColor("#e84393"), QColor("#74b9ff"),
        QColor("#a29bfe"), QColor("#fab1a0"), QColor("#81ecec"),
        QColor("#dfe6e9"), QColor("#55efc4"), QColor("#ff7675")
    };
    setMinimumSize(400, 450);
}

void ChartWidget::setData(const QList<QPair<QString, int>> &data)
{
    chartData = data;
    update(); // 触发重绘
}

void ChartWidget::paintEvent(QPaintEvent *)
{
    if (chartData.isEmpty()) {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QColor("#636e72"));
        p.setFont(QFont("Sans", 16));
        p.drawText(rect(), Qt::AlignCenter, "暂无选课数据，请先添加课程和选课记录");
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    int padding = 60;      // 边距
    int titleHeight = 50;  // 标题高度

    // ===== 绘制标题 =====
    painter.setPen(QColor("#2c3e50"));
    painter.setFont(QFont("Sans", 18, QFont::Bold));
    painter.drawText(QRect(0, 10, w, titleHeight), Qt::AlignCenter, "各课程选课人数统计");

    // ===== 绘制柱状图区域 =====
    int chartTop = titleHeight + 20;
    int chartBottom = h - padding;
    int chartLeft = padding + 40; // 左侧留空间给Y轴标签
    int chartRight = w - padding;
    int chartWidth = chartRight - chartLeft;
    int chartHeight = chartBottom - chartTop;

    // 计算最大值
    int maxVal = 0;
    for (const auto &d : chartData) {
        if (d.second > maxVal) maxVal = d.second;
    }
    if (maxVal == 0) maxVal = 10; // 避免为0

    // 绘制Y轴刻度线
    painter.setPen(QColor("#b2bec3"));
    painter.setFont(QFont("Sans", 10));
    int ySteps = 5;
    for (int i = 0; i <= ySteps; i++) {
        int y = chartBottom - (i * chartHeight / ySteps);
        painter.drawLine(chartLeft, y, chartRight, y);
        int val = (i * maxVal / ySteps);
        painter.setPen(QColor("#636e72"));
        painter.drawText(QRect(0, y - 10, chartLeft - 10, 20), Qt::AlignRight | Qt::AlignVCenter, QString::number(val));
        painter.setPen(QColor("#b2bec3"));
    }

    // 绘制轴线
    painter.setPen(QPen(QColor("#2c3e50"), 2));
    painter.drawLine(chartLeft, chartBottom, chartRight, chartBottom); // X轴
    painter.drawLine(chartLeft, chartTop, chartLeft, chartBottom);     // Y轴

    // 绘制柱状图
    int barCount = qMin(chartData.size(), 12);
    if (barCount == 0) return;

    int barSpacing = 20;
    int totalSpacing = (barCount + 1) * barSpacing;
    int barWidth = (chartWidth - totalSpacing) / barCount;
    if (barWidth < 20) barWidth = 20;

    for (int i = 0; i < barCount; i++) {
        int x = chartLeft + barSpacing + i * (barWidth + barSpacing);
        double ratio = (double)chartData[i].second / maxVal;
        int barH = (int)(ratio * (chartHeight - 20));
        int y = chartBottom - barH;

        // 绘制柱子（带渐变效果）
        QColor barColor = colors[i % colors.size()];
        QLinearGradient grad(x, y, x, chartBottom);
        grad.setColorAt(0, barColor.lighter(120));
        grad.setColorAt(1, barColor);
        painter.setBrush(grad);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(x, y, barWidth, barH, 4, 4);

        // 柱顶显示数值
        painter.setPen(QColor("#2c3e50"));
        painter.setFont(QFont("Sans", 10, QFont::Bold));
        painter.drawText(QRect(x, y - 22, barWidth, 20), Qt::AlignCenter, QString::number(chartData[i].second));

        // X轴标签：课程名（过长的截断）
        painter.setFont(QFont("Sans", 9));
        QString label = chartData[i].first;
        if (label.length() > 6) label = label.left(5) + "..";
        painter.drawText(QRect(x, chartBottom + 5, barWidth, 30), Qt::AlignCenter, label);
    }

    // ===== 绘制右下角饼图（选课分布概览）=====
    int pieSize = qMin(160, qMin(w, h) / 4);
    int pieX = w - pieSize - padding;
    int pieY = chartTop;
    QRect pieRect(pieX, pieY, pieSize, pieSize);

    // 饼图标题
    painter.setPen(QColor("#636e72"));
    painter.setFont(QFont("Sans", 9));
    painter.drawText(QRect(pieX, pieY - 18, pieSize, 16), Qt::AlignCenter, "选课占比");

    // 绘制饼图
    int total = 0;
    for (const auto &d : chartData) total += d.second;
    if (total > 0) {
        int startAngle = 0;
        for (int i = 0; i < barCount && i < chartData.size(); i++) {
            int spanAngle = (int)(chartData[i].second * 360 * 16 / total);
            if (spanAngle == 0) continue;
            painter.setBrush(colors[i % colors.size()]);
            painter.setPen(Qt::white);
            painter.drawPie(pieRect, startAngle, spanAngle);
            startAngle += spanAngle;
        }
    }
}

// =====================================================================
// AdminWindow 实现 - 管理员主窗口
// =====================================================================

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminWindow)
    , chartWidget(nullptr)
{
    ui->setupUi(this);

    // 初始化五个Tab页
    initCourseTab();
    initSelectionTab();
    initStudentTab();
    initChartTab();
    initAuditTab();

    // 显示状态栏提示
    statusBar()->showMessage("欢迎使用教务选课管理系统 - 管理员后台");
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

// =====================================================================
// 初始化 - 课程管理Tab
// =====================================================================
void AdminWindow::initCourseTab()
{
    // 设置课程表格列
    QStringList headers = {"课程编号", "课程名称", "授课教师", "学分", "容量", "已选人数", "状态"};
    ui->courseTable->setColumnCount(headers.size());
    ui->courseTable->setHorizontalHeaderLabels(headers);
    ui->courseTable->horizontalHeader()->setStretchLastSection(true);
    ui->courseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->courseTable->setSortingEnabled(true);

    refreshCourseTable();
}

// =====================================================================
// 初始化 - 选课管理Tab
// =====================================================================
void AdminWindow::initSelectionTab()
{
    // 设置选课表格列
    QStringList headers = {"记录ID", "学生ID", "学生姓名", "课程编号", "课程名称", "选课时间"};
    ui->selectionTable->setColumnCount(headers.size());
    ui->selectionTable->setHorizontalHeaderLabels(headers);
    ui->selectionTable->horizontalHeader()->setStretchLastSection(true);
    ui->selectionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 初始化筛选下拉框：加载所有课程
    ui->selectionFilterCombo->addItem("全部课程", QString());
    QList<Course> courses = CourseDao::findAll();
    for (const auto &c : courses) {
        ui->selectionFilterCombo->addItem(c.course_name + " (" + c.course_id + ")", c.course_id);
    }

    refreshSelectionTable();
}

// =====================================================================
// 初始化 - 学生管理Tab
// =====================================================================
void AdminWindow::initStudentTab()
{
    // 设置学生表格列
    QStringList headers = {"ID", "学号", "姓名", "角色"};
    ui->studentTable->setColumnCount(headers.size());
    ui->studentTable->setHorizontalHeaderLabels(headers);
    ui->studentTable->horizontalHeader()->setStretchLastSection(true);
    ui->studentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 隐藏第一列（数据库自增ID）作为内部标识
    ui->studentTable->setColumnHidden(0, true);

    refreshStudentTable();
}

// =====================================================================
// 刷新学生表格
// =====================================================================
void AdminWindow::refreshStudentTable(const QString &keyword)
{
    QList<StudentInfo> students;
    if (keyword.isEmpty()) {
        students = StudentDao::findAllStudents();
    } else {
        students = StudentDao::search(keyword);
    }

    ui->studentTable->setRowCount(0);
    ui->studentTable->setRowCount(students.size());

    for (int i = 0; i < students.size(); i++) {
        const StudentInfo &s = students[i];
        ui->studentTable->setItem(i, 0, new QTableWidgetItem(QString::number(s.dbId)));
        ui->studentTable->setItem(i, 1, new QTableWidgetItem(s.userId));
        ui->studentTable->setItem(i, 2, new QTableWidgetItem(s.userName));
        QString roleText = (s.role == "1") ? "学生" : (s.role == "0" ? "管理员" : "教师");
        ui->studentTable->setItem(i, 3, new QTableWidgetItem(roleText));
    }
}

// =====================================================================
// 学生管理 - 搜索按钮
// =====================================================================
void AdminWindow::on_studentSearchBtn_clicked()
{
    QString keyword = ui->studentSearchEdit->text().trimmed();
    refreshStudentTable(keyword);
}

// =====================================================================
// 学生管理 - 新增学生
// =====================================================================
void AdminWindow::on_addStudentBtn_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("新增学生");
    dialog.setFixedSize(420, 280);
    dialog.setStyleSheet("QDialog { background: #f5f6fa; } QLabel { font-size: 13px; }");

    QFormLayout form(&dialog);

    QLineEdit *idEdit = new QLineEdit(&dialog);
    idEdit->setPlaceholderText("请输入学号，如 20240001");
    idEdit->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("学号：", idEdit);

    QLineEdit *nameEdit = new QLineEdit(&dialog);
    nameEdit->setPlaceholderText("请输入学生姓名");
    nameEdit->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("姓名：", nameEdit);

    QLineEdit *passEdit = new QLineEdit(&dialog);
    passEdit->setPlaceholderText("请输入初始密码（至少8位含大小写字母和数字）");
    passEdit->setEchoMode(QLineEdit::Password);
    passEdit->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("初始密码：", passEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText("确定");
    buttons->button(QDialogButtonBox::Cancel)->setText("取消");
    buttons->button(QDialogButtonBox::Ok)->setStyleSheet("background: #0984e3; color: white; padding: 8px 20px;");
    buttons->button(QDialogButtonBox::Cancel)->setStyleSheet("padding: 8px 20px;");
    form.addRow(buttons);

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString userId = idEdit->text().trimmed();
        QString userName = nameEdit->text().trimmed();
        QString password = passEdit->text();

        // 基本校验
        if (userId.isEmpty() || userName.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "错误", "所有字段均不能为空！");
            return;
        }
        if (password.length() < 8) {
            QMessageBox::warning(this, "错误", "密码至少8位！");
            return;
        }

        StudentInfo stu;
        stu.userId = userId;
        stu.userName = userName;
        stu.role = "1";

        QString err = StudentDao::insertStudent(stu, password);
        if (err.isEmpty()) {
            QMessageBox::information(this, "成功", "学生添加成功！");
            refreshStudentTable();
        } else {
            QMessageBox::warning(this, "错误", err);
        }
    }
}

// =====================================================================
// 学生管理 - 编辑学生
// =====================================================================
void AdminWindow::on_editStudentBtn_clicked()
{
    int row = ui->studentTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的学生！");
        return;
    }

    int dbId = ui->studentTable->item(row, 0)->text().toInt();
    auto stuOpt = StudentDao::findById(dbId);
    if (!stuOpt.has_value()) {
        QMessageBox::warning(this, "错误", "学生不存在！");
        return;
    }

    StudentInfo existing = stuOpt.value();

    QDialog dialog(this);
    dialog.setWindowTitle("编辑学生信息");
    dialog.setFixedSize(420, 220);
    dialog.setStyleSheet("QDialog { background: #f5f6fa; } QLabel { font-size: 13px; }");

    QFormLayout form(&dialog);

    QLineEdit *idEdit = new QLineEdit(existing.userId, &dialog);
    idEdit->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("学号：", idEdit);

    QLineEdit *nameEdit = new QLineEdit(existing.userName, &dialog);
    nameEdit->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("姓名：", nameEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText("确定");
    buttons->button(QDialogButtonBox::Cancel)->setText("取消");
    buttons->button(QDialogButtonBox::Ok)->setStyleSheet("background: #0984e3; color: white; padding: 8px 20px;");
    buttons->button(QDialogButtonBox::Cancel)->setStyleSheet("padding: 8px 20px;");
    form.addRow(buttons);

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        existing.userId = idEdit->text().trimmed();
        existing.userName = nameEdit->text().trimmed();

        if (existing.userId.isEmpty() || existing.userName.isEmpty()) {
            QMessageBox::warning(this, "错误", "学号和姓名不能为空！");
            return;
        }

        if (StudentDao::updateStudent(existing)) {
            QMessageBox::information(this, "成功", "学生信息更新成功！");
            refreshStudentTable();
        } else {
            QMessageBox::warning(this, "错误", "更新失败，学号可能重复！");
        }
    }
}

// =====================================================================
// 学生管理 - 删除学生
// =====================================================================
void AdminWindow::on_deleteStudentBtn_clicked()
{
    int row = ui->studentTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的学生！");
        return;
    }

    int dbId = ui->studentTable->item(row, 0)->text().toInt();
    QString userName = ui->studentTable->item(row, 2)->text();

    int ret = QMessageBox::question(this, "确认删除",
        QString("确定要删除学生 \"%1\" 吗？\n此操作将同时删除该学生的所有选课记录，不可恢复！").arg(userName),
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (StudentDao::removeStudent(dbId)) {
            QMessageBox::information(this, "成功", "学生删除成功！");
            refreshStudentTable();
        } else {
            QMessageBox::warning(this, "错误", "删除失败，请重试！");
        }
    }
}

// =====================================================================
// 初始化 - 数据可视化Tab
// =====================================================================
void AdminWindow::initChartTab()
{
    // 将自定义ChartWidget嵌入到chartContainer中
    chartWidget = new ChartWidget(ui->chartContainer);
    QVBoxLayout *layout = new QVBoxLayout(ui->chartContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartWidget);

    // 首次加载数据
    auto stats = AdminService::getEnrollmentStats();
    chartWidget->setData(stats);
}

// =====================================================================
// 初始化 - 课程审核Tab
// =====================================================================
void AdminWindow::initAuditTab()
{
    // 设置审核表格列
    QStringList headers = {"申请ID", "课程名称", "教师姓名", "学分", "容量", "提交时间", "状态"};
    ui->auditTable->setColumnCount(headers.size());
    ui->auditTable->setHorizontalHeaderLabels(headers);
    ui->auditTable->horizontalHeader()->setStretchLastSection(true);
    ui->auditTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    refreshAuditTable();
}

// =====================================================================
// 课程管理 - 刷新课程表格
// =====================================================================
void AdminWindow::refreshCourseTable(const QString &keyword)
{
    QList<Course> courses;
    if (keyword.isEmpty()) {
        courses = CourseService::getAllCourses();
    } else {
        courses = CourseService::searchCourses(keyword);
    }

    ui->courseTable->setRowCount(0);
    ui->courseTable->setRowCount(courses.size());

    for (int i = 0; i < courses.size(); i++) {
        const Course &c = courses[i];
        ui->courseTable->setItem(i, 0, new QTableWidgetItem(c.course_id));
        ui->courseTable->setItem(i, 1, new QTableWidgetItem(c.course_name));
        ui->courseTable->setItem(i, 2, new QTableWidgetItem(c.teacher_name));
        ui->courseTable->setItem(i, 3, new QTableWidgetItem(QString::number(c.credit, 'f', 1)));
        ui->courseTable->setItem(i, 4, new QTableWidgetItem(QString::number(c.max_capacity)));
        ui->courseTable->setItem(i, 5, new QTableWidgetItem(QString::number(c.current_enrolled)));
        ui->courseTable->setItem(i, 6, new QTableWidgetItem(c.status == 1 ? "开课中" : "已停课"));
    }
}

// =====================================================================
// 课程管理 - 搜索按钮
// =====================================================================
void AdminWindow::on_courseSearchBtn_clicked()
{
    QString keyword = ui->courseSearchEdit->text().trimmed();
    refreshCourseTable(keyword);
}

// =====================================================================
// 课程管理 - 新增课程
// =====================================================================
void AdminWindow::on_addCourseBtn_clicked()
{
    QString courseId, courseName, teacherName;
    float credit = 2.0;
    int maxCapacity = 50;
    if (showCourseDialog(false, courseId, courseName, teacherName, credit, maxCapacity)) {
        Course course;
        course.course_id = courseId;
        course.course_name = courseName;
        course.teacher_name = teacherName;
        course.credit = credit;
        course.max_capacity = maxCapacity;
        course.current_enrolled = 0;
        course.status = 1;

        QString err = CourseService::addCourse(course);
        if (err.isEmpty()) {
            QMessageBox::information(this, "成功", "课程添加成功！");
            refreshCourseTable();
            // 同步刷新选课管理的筛选下拉框
            initSelectionTab();
        } else {
            QMessageBox::warning(this, "错误", err);
        }
    }
}

// =====================================================================
// 课程管理 - 编辑课程
// =====================================================================
void AdminWindow::on_editCourseBtn_clicked()
{
    int row = ui->courseTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的课程！");
        return;
    }

    QString courseId = ui->courseTable->item(row, 0)->text();
    auto courseOpt = CourseDao::findById(courseId);
    if (!courseOpt.has_value()) {
        QMessageBox::warning(this, "错误", "课程不存在！");
        return;
    }

    Course existing = courseOpt.value();
    QString newName = existing.course_name;
    QString newTeacher = existing.teacher_name;
    float newCredit = existing.credit;
    int newMax = existing.max_capacity;

    if (showCourseDialog(true, courseId, newName, newTeacher, newCredit, newMax)) {
        // 课程ID不允许修改，其余字段可修改
        existing.course_name = newName;
        existing.teacher_name = newTeacher;
        existing.credit = newCredit;
        existing.max_capacity = newMax;

        QString err = CourseService::updateCourse(existing);
        if (err.isEmpty()) {
            QMessageBox::information(this, "成功", "课程信息更新成功！");
            refreshCourseTable();
        } else {
            QMessageBox::warning(this, "错误", err);
        }
    }
}

// =====================================================================
// 课程管理 - 删除课程
// =====================================================================
void AdminWindow::on_deleteCourseBtn_clicked()
{
    int row = ui->courseTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的课程！");
        return;
    }

    QString courseId = ui->courseTable->item(row, 0)->text();
    QString courseName = ui->courseTable->item(row, 1)->text();

    int ret = QMessageBox::question(this, "确认删除",
        QString("确定要删除课程 \"%1\" (%2) 吗？\n此操作将同时删除该课程的所有选课记录，不可恢复！")
            .arg(courseName, courseId),
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (CourseService::deleteCourse(courseId)) {
            QMessageBox::information(this, "成功", "课程删除成功！");
            refreshCourseTable();
        } else {
            QMessageBox::warning(this, "错误", "删除失败，请重试！");
        }
    }
}

// =====================================================================
// 课程编辑弹窗 - 新增/编辑课程共用
// =====================================================================
bool AdminWindow::showCourseDialog(bool isEdit, QString &courseId,
                                   QString &courseName, QString &teacherName,
                                   float &credit, int &maxCapacity)
{
    QDialog dialog(this);
    dialog.setWindowTitle(isEdit ? "编辑课程" : "新增课程");
    dialog.setFixedSize(420, 320);
    dialog.setStyleSheet("QDialog { background: #f5f6fa; } QLabel { font-size: 13px; }");

    QFormLayout form(&dialog);

    QLineEdit *idEdit = new QLineEdit(courseId, &dialog);
    idEdit->setPlaceholderText("请输入课程编号，如 CS101");
    idEdit->setEnabled(!isEdit); // 编辑模式下不允许修改ID
    idEdit->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("课程编号：", idEdit);

    QLineEdit *nameEdit = new QLineEdit(courseName, &dialog);
    nameEdit->setPlaceholderText("请输入课程名称");
    nameEdit->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("课程名称：", nameEdit);

    QLineEdit *teacherEdit = new QLineEdit(teacherName, &dialog);
    teacherEdit->setPlaceholderText("请输入授课教师姓名");
    teacherEdit->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("授课教师：", teacherEdit);

    QDoubleSpinBox *creditSpin = new QDoubleSpinBox(&dialog);
    creditSpin->setRange(0, 20);
    creditSpin->setSingleStep(0.5);
    creditSpin->setValue(credit);
    creditSpin->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("学分：", creditSpin);

    QSpinBox *capacitySpin = new QSpinBox(&dialog);
    capacitySpin->setRange(1, 500);
    capacitySpin->setValue(maxCapacity);
    capacitySpin->setStyleSheet("padding: 8px; border: 1px solid #dcdde1; border-radius: 4px;");
    form.addRow("课程容量：", capacitySpin);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText("确定");
    buttons->button(QDialogButtonBox::Cancel)->setText("取消");
    buttons->button(QDialogButtonBox::Ok)->setStyleSheet("background: #0984e3; color: white; padding: 8px 20px;");
    buttons->button(QDialogButtonBox::Cancel)->setStyleSheet("padding: 8px 20px;");
    form.addRow(buttons);

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        courseId = idEdit->text().trimmed();
        courseName = nameEdit->text().trimmed();
        teacherName = teacherEdit->text().trimmed();
        credit = creditSpin->value();
        maxCapacity = capacitySpin->value();
        return true;
    }
    return false;
}

// =====================================================================
// 选课管理 - 刷新选课表格
// =====================================================================
void AdminWindow::refreshSelectionTable(const QString &courseId)
{
    QList<SelectionRecord> records;
    if (courseId.isEmpty()) {
        records = AdminService::getAllSelections();
    } else {
        records = AdminService::getSelectionsByCourse(courseId);
    }

    ui->selectionTable->setRowCount(0);
    ui->selectionTable->setRowCount(records.size());

    for (int i = 0; i < records.size(); i++) {
        const SelectionRecord &r = records[i];
        ui->selectionTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.id)));
        ui->selectionTable->setItem(i, 1, new QTableWidgetItem(r.stuId));
        ui->selectionTable->setItem(i, 2, new QTableWidgetItem(r.stuName));
        ui->selectionTable->setItem(i, 3, new QTableWidgetItem(r.courseId));
        ui->selectionTable->setItem(i, 4, new QTableWidgetItem(r.courseName));
        ui->selectionTable->setItem(i, 5, new QTableWidgetItem(r.selectTime.toString("yyyy-MM-dd hh:mm:ss")));
    }
}

// =====================================================================
// 选课管理 - 筛选课程变化
// =====================================================================
void AdminWindow::on_selectionFilterCombo_currentIndexChanged(int index)
{
    QString courseId = ui->selectionFilterCombo->itemData(index).toString();
    refreshSelectionTable(courseId);
}

// =====================================================================
// 选课管理 - 刷新按钮
// =====================================================================
void AdminWindow::on_refreshSelectionBtn_clicked()
{
    // 重新加载课程下拉框
    ui->selectionFilterCombo->blockSignals(true);
    QString currentId = ui->selectionFilterCombo->currentData().toString();
    ui->selectionFilterCombo->clear();
    ui->selectionFilterCombo->addItem("全部课程", QString());
    QList<Course> courses = CourseDao::findAll();
    for (const auto &c : courses) {
        ui->selectionFilterCombo->addItem(c.course_name + " (" + c.course_id + ")", c.course_id);
    }
    // 恢复选中
    int idx = ui->selectionFilterCombo->findData(currentId);
    if (idx >= 0) ui->selectionFilterCombo->setCurrentIndex(idx);
    ui->selectionFilterCombo->blockSignals(false);

    refreshSelectionTable(currentId);
}

// =====================================================================
// 选课管理 - 退选
// =====================================================================
void AdminWindow::on_removeSelectionBtn_clicked()
{
    int row = ui->selectionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要退选的记录！");
        return;
    }

    int selectionId = ui->selectionTable->item(row, 0)->text().toInt();
    QString stuName = ui->selectionTable->item(row, 2)->text();
    QString courseName = ui->selectionTable->item(row, 4)->text();

    int ret = QMessageBox::question(this, "确认退选",
        QString("确定要为学生 \"%1\" 退选课程 \"%2\" 吗？").arg(stuName, courseName),
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (AdminService::removeSelection(selectionId)) {
            QMessageBox::information(this, "成功", "退选操作成功！");
            refreshSelectionTable(ui->selectionFilterCombo->currentData().toString());
            refreshCourseTable();
        } else {
            QMessageBox::warning(this, "错误", "退选失败，请重试！");
        }
    }
}

// =====================================================================
// 数据可视化 - 刷新图表
// =====================================================================
void AdminWindow::on_refreshChartBtn_clicked()
{
    auto stats = AdminService::getEnrollmentStats();
    chartWidget->setData(stats);
    statusBar()->showMessage("图表数据已刷新", 3000);
}

// =====================================================================
// 课程审核 - 刷新审核表格
// =====================================================================
void AdminWindow::refreshAuditTable()
{
    QList<ApplicationRecord> apps = AdminService::getAllApplications();
    ui->auditTable->setRowCount(0);
    ui->auditTable->setRowCount(apps.size());

    for (int i = 0; i < apps.size(); i++) {
        const ApplicationRecord &a = apps[i];
        ui->auditTable->setItem(i, 0, new QTableWidgetItem(QString::number(a.id)));
        ui->auditTable->setItem(i, 1, new QTableWidgetItem(a.courseName));
        ui->auditTable->setItem(i, 2, new QTableWidgetItem(a.teacherName));
        ui->auditTable->setItem(i, 3, new QTableWidgetItem(QString::number(a.credit, 'f', 1)));
        ui->auditTable->setItem(i, 4, new QTableWidgetItem(QString::number(a.maxCapacity)));
        ui->auditTable->setItem(i, 5, new QTableWidgetItem(a.submitTime.toString("yyyy-MM-dd hh:mm")));

        QString statusText;
        QColor statusColor;
        switch (a.status) {
        case 0: statusText = "待审核"; statusColor = QColor("#fdcb6e"); break;
        case 1: statusText = "已通过"; statusColor = QColor("#00b894"); break;
        case 2: statusText = "已拒绝"; statusColor = QColor("#e17055"); break;
        default: statusText = "未知";
        }
        QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);
        statusItem->setForeground(statusColor);
        ui->auditTable->setItem(i, 6, statusItem);
    }
}

// =====================================================================
// 课程审核 - 点击查看申请详情
// =====================================================================
void AdminWindow::on_auditTable_itemClicked(QTableWidgetItem *item)
{
    int row = item->row();
    int appId = ui->auditTable->item(row, 0)->text().toInt();
    auto appOpt = ApplicationDao::findById(appId);
    if (!appOpt.has_value()) return;

    ApplicationRecord app = appOpt.value();
    QString statusText;
    switch (app.status) {
    case 0: statusText = "待审核"; break;
    case 1: statusText = "已通过"; break;
    case 2: statusText = "已拒绝"; break;
    }

    QString detail = QString(
        "═══════ 课程申请详情 ═══════\n\n"
        "申请编号：%1\n"
        "课程名称：%2\n"
        "授课教师：%3 (ID: %4)\n"
        "课程学分：%5\n"
        "课程容量：%6 人\n"
        "提交时间：%7\n"
        "审核状态：%8\n\n"
        "═══════ 申请理由 ═══════\n"
        "%9"
    ).arg(QString::number(app.id),
          app.courseName,
          app.teacherName,
          app.teacherId,
          QString::number(app.credit, 'f', 1),
          QString::number(app.maxCapacity),
          app.submitTime.toString("yyyy-MM-dd hh:mm:ss"),
          statusText,
          app.reason.isEmpty() ? "（无）" : app.reason);

    ui->auditDetailText->setText(detail);
}

// =====================================================================
// 课程审核 - 通过申请
// =====================================================================
void AdminWindow::on_approveBtn_clicked()
{
    int row = ui->auditTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先在左侧列表中选择一条待审核的申请！");
        return;
    }

    int appId = ui->auditTable->item(row, 0)->text().toInt();
    QString statusText = ui->auditTable->item(row, 6)->text();
    if (statusText != "待审核") {
        QMessageBox::warning(this, "提示", "该申请已被处理，无法重复审核！");
        return;
    }

    QString err = AdminService::approveApplication(appId);
    if (err.isEmpty()) {
        QMessageBox::information(this, "成功", "课程申请审核通过，已自动创建课程！");
        refreshAuditTable();
        refreshCourseTable();
    } else {
        QMessageBox::warning(this, "错误", err);
    }
}

// =====================================================================
// 课程审核 - 拒绝申请
// =====================================================================
void AdminWindow::on_rejectBtn_clicked()
{
    int row = ui->auditTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先在左侧列表中选择一条待审核的申请！");
        return;
    }

    int appId = ui->auditTable->item(row, 0)->text().toInt();
    QString statusText = ui->auditTable->item(row, 6)->text();
    if (statusText != "待审核") {
        QMessageBox::warning(this, "提示", "该申请已被处理，无法重复审核！");
        return;
    }

    int ret = QMessageBox::question(this, "确认拒绝",
        "确定要拒绝该课程申请吗？",
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (AdminService::rejectApplication(appId)) {
            QMessageBox::information(this, "成功", "已拒绝该课程申请！");
            refreshAuditTable();
        } else {
            QMessageBox::warning(this, "错误", "操作失败，请重试！");
        }
    }
}

// =====================================================================
// Tab切换 - 切换时刷新对应数据
// =====================================================================
void AdminWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0: // 课程管理
        refreshCourseTable();
        break;
    case 1: // 选课管理
        refreshSelectionTable(ui->selectionFilterCombo->currentData().toString());
        break;
    case 2: // 学生管理
        refreshStudentTable();
        break;
    case 3: // 数据可视化
        on_refreshChartBtn_clicked();
        break;
    case 4: // 课程审核
        refreshAuditTable();
        break;
    }
}
