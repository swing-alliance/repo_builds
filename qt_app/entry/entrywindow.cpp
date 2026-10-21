#include "entrywindow.h"
#include "ui_entrywindow.h"
#include<core/apptime.h>
#include<core/paths.h>
#include<QTimer>
#include<QString>
#include<QPushButton>
#include<QDebug>
#include<virtual/v_app.h>
#include <timewindow.h>
#include<QAction>
#include "test_login.h"
#include <model/fund_model.h>
#include "fund_pannel_widget.h"
EntryWindow::EntryWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EntryWindow)
{
    ui->setupUi(this);
    this->main_page_timer=new QTimer(this);
    this->main_page_timer->start(500);

    connect(this->main_page_timer,&QTimer::timeout,this,[this](){this->ui->t_label->setText(QString::fromStdString(core::getnow_string()));});
    connect(ui->action_v_time,&QAction::triggered,this,[this](){this->t=new TimeWindow();this->t->show();});
    connect(ui->actionlow_load,&QAction::triggered,this,[this](){
        if(this->g_fund_manager==nullptr){this->g_fund_manager=new model::datas_fund_manager;}
        this->g_fund_manager->fast_low_level_load_data(core::funds_row_dir);});
    connect(ui->actionstd_load,&QAction::triggered,this,[this](){
        if(this->g_fund_manager==nullptr){this->g_fund_manager=new model::datas_fund_manager;}
        this->g_fund_manager->std_load_data(core::funds_row_dir);});
    connect(ui->actiondir_main,&QAction::triggered,this,[this](){this->dir_main_page();});
    connect(ui->actiondir_fund_pannel,&QAction::triggered,this,[this](){this->dir_fund_pannel();});

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

EntryWindow::~EntryWindow(){delete ui;}

void EntryWindow::dir_main_page(){ui->stackedWidget->setCurrentIndex(0);}

void EntryWindow::dir_fund_pannel()
{

    if (!fund_pannel) {
        fund_pannel = new fund_pannel_widget(this);
        if (this->g_fund_manager != nullptr) {
            fund_pannel->g_fund_manager = this->g_fund_manager;
        } else {
            fund_pannel->g_fund_manager = new model::datas_fund_manager();
            this->g_fund_manager = fund_pannel->g_fund_manager;  // 保持唯一
            fund_pannel->g_fund_manager->fast_low_level_load_data(core::funds_row_dir);
        }
        fund_pannel->connect_funds_ui();
        ui->stackedWidget->addWidget(fund_pannel);
    }
    ui->stackedWidget->setCurrentWidget(fund_pannel);
}


