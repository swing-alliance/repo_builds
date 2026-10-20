#include "entrywindow.h"
#include "ui_entrywindow.h"
#include<core/apptime.h>
#include<QTimer>
#include<QString>
#include<QPushButton>
#include<QDebug>
#include<virtual/v_app.h>
#include <timewindow.h>
#include<QAction>
#include "test_login.h"
EntryWindow::EntryWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EntryWindow)
{
    ui->setupUi(this);
    this->timer=new QTimer(this);
    this->timer->start(500);

    connect(this->timer,&QTimer::timeout,this,[this](){
        this->ui->t_label->setText(QString::fromStdString(core::getnow_string()));
    });
    connect(ui->action_v_time,&QAction::triggered,this,[this](){
        this->t=new TimeWindow();
        this->t->show();
    });

    //======测试界=====
    connect(ui->dir_login_test, &QAction::triggered, this, [this](){
        if (!loginPage) {
            loginPage = new test_login(this);
            ui->stackedWidget->addWidget(loginPage); // 仅添加一次
        }
        ui->stackedWidget->setCurrentWidget(loginPage); // 毫秒级切换，零负担
    });
    connect(ui->dir_main_test,&QAction::triggered,this,[this](){
        ui->stackedWidget->setCurrentIndex(0);
    });

    //======测试界=====


}

EntryWindow::~EntryWindow()
{
    delete ui;
}

void EntryWindow::dir_main_page()
{
    ui->stackedWidget->setCurrentIndex(0);
}


