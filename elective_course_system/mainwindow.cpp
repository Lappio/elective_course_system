#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include"db/dbmanager.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);


    if(DbManager::instance().connect()){
        QMessageBox::information(this,"连接成功","Nice");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginPushButton_clicked(bool checked)
{
    // 接收用户名和密码
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
}

