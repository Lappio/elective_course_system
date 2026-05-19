#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class AdminWindow;
}
QT_END_NAMESPACE

// 自定义图表绘制控件 - 使用QPainter绘制柱状图和饼图
class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = nullptr);
    // 设置图表数据：课程名和选课人数
    void setData(const QList<QPair<QString, int>> &data);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QList<QPair<QString, int>> chartData;
    // 预定义颜色方案
    QList<QColor> colors;
};

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow() override;

private slots:
    // ===== 课程管理 =====
    void on_courseSearchBtn_clicked();       // 搜索课程
    void on_addCourseBtn_clicked();          // 新增课程
    void on_editCourseBtn_clicked();         // 编辑课程
    void on_deleteCourseBtn_clicked();       // 删除课程

    // ===== 选课管理 =====
    void on_refreshSelectionBtn_clicked();   // 刷新选课列表
    void on_selectionFilterCombo_currentIndexChanged(int index); // 筛选课程
    void on_removeSelectionBtn_clicked();    // 退选

    // ===== 学生管理 =====
    void on_studentSearchBtn_clicked();      // 搜索学生
    void on_addStudentBtn_clicked();         // 新增学生
    void on_editStudentBtn_clicked();        // 编辑学生
    void on_deleteStudentBtn_clicked();      // 删除学生

    // ===== 数据可视化 =====
    void on_refreshChartBtn_clicked();       // 刷新图表

    // ===== 课程审核 =====
    void on_auditTable_itemClicked(QTableWidgetItem *item); // 点击查看申请详情
    void on_approveBtn_clicked();            // 通过申请
    void on_rejectBtn_clicked();             // 拒绝申请

    // ===== Tab切换 =====
    void on_tabWidget_currentChanged(int index);

private:
    Ui::AdminWindow *ui;
    ChartWidget *chartWidget;                // 自定义图表控件

    // ===== 初始化各个Tab页 =====
    void initCourseTab();                    // 初始化课程管理Tab
    void initSelectionTab();                 // 初始化选课管理Tab
    void initStudentTab();                   // 初始化学生管理Tab
    void initChartTab();                     // 初始化图表Tab
    void initAuditTab();                     // 初始化审核Tab

    // ===== 刷新数据 =====
    void refreshCourseTable(const QString &keyword = QString()); // 刷新课程表格
    void refreshSelectionTable(const QString &courseId = QString()); // 刷新选课表格
    void refreshStudentTable(const QString &keyword = QString()); // 刷新学生表格
    void refreshAuditTable();                // 刷新审核表格

    // ===== 课程编辑弹窗 =====
    bool showCourseDialog(bool isEdit, QString &courseId,
                          QString &courseName, QString &teacherName,
                          float &credit, int &maxCapacity);
};

#endif // ADMINWINDOW_H
