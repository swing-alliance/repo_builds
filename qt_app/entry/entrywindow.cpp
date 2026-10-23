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

#include <model/fund_model.h>
#include "fund_pannel_widget.h"
#include "cal_param_widget.h"
EntryWindow::EntryWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EntryWindow)
{
    ui->setupUi(this);
    this->main_page_timer=new QTimer(this);
    this->main_page_timer->start(500);

    connect(this->main_page_timer,&QTimer::timeout,this,[this](){this->ui->t_label->setText(QString::fromStdString(core::getnow_string()));});
    connect(ui->action_v_time, &QAction::triggered, this, [this]() {
        if (this->t != nullptr) {
            this->t->raise();
            this->t->activateWindow();
            return;
        }
        this->t = new TimeWindow(nullptr);
        connect(this->t, &TimeWindow::time_updated, this, [this]() {
            if (this->g_fund_manager != nullptr) {
                this->g_fund_manager->fast_low_level_load_data(core::funds_row_dir);
            }
            if (this->fund_pannel != nullptr && ui->stackedWidget->currentWidget() == this->fund_pannel) {
                this->fund_pannel->refresh_funds_ui();
            } else {
                qDebug() << "当前不在基金面板，后台数据已更新，待切过来时自动刷新。";
            }
        });
        connect(this->t, &QObject::destroyed, this, [this]() {this->t = nullptr;});this->t->show();});
    connect(ui->actionlow_load,&QAction::triggered,this,[this](){
        if(this->g_fund_manager==nullptr){this->g_fund_manager=new model::datas_fund_manager;}
        this->g_fund_manager->fast_low_level_load_data(core::funds_row_dir);});
    connect(ui->actionstd_load,&QAction::triggered,this,[this](){                                   //标准加载的按钮
        if(this->g_fund_manager==nullptr){this->g_fund_manager=new model::datas_fund_manager;}
        this->g_fund_manager->std_load_data(core::funds_row_dir);});
    connect(ui->actiondir_main,&QAction::triggered,this,[this](){this->dir_main_page();});           //导航到主的按钮
    connect(ui->actiondir_fund_pannel,&QAction::triggered,this,[this](){this->dir_fund_pannel();});  //导航到鸡控制办的按钮
    connect(ui->instant_start_btn, &QPushButton::clicked, this, [this](){   //快速开始
        ui->actionstd_load->setEnabled(false);
        ui->actiondir_fund_pannel->setEnabled(false);
        ui->actionstd_load->trigger();
        ui->actiondir_fund_pannel->trigger();
        ui->actionstd_load->setEnabled(true);
        ui->actiondir_fund_pannel->setEnabled(true);
    });
    connect(ui->action_cal_resort,&QAction::triggered,this,[this](){
        // cal_param_Widget * cal_win=new cal_param_Widget(this->g_fund_manager,this);
        // cal_win->show();
    });
    //======测试界=====


    //======测试界=====


}

EntryWindow::~EntryWindow(){delete ui;}

void EntryWindow::dir_main_page(){ui->stackedWidget->setCurrentIndex(0);}

void EntryWindow::dir_fund_pannel()
{
    if (!fund_pannel) {
        fund_pannel = new fund_pannel_widget(this);
        connect(fund_pannel, &fund_pannel_widget::on_card_dir_chart,this, [this](model::fund_model* fund_ptr) {
                    if (!fund_ptr) return;
                    if (chart_widget) {
                        ui->stackedWidget->removeWidget(chart_widget);
                        chart_widget->deleteLater();
                        chart_widget = nullptr;
                    }
                    chart_widget = new ChartWidget(fund_ptr,QString::fromStdString(fund_ptr->fund_name),QString::fromStdString(fund_ptr->fund_code),this);
                    connect(chart_widget, &ChartWidget::pop_here,this, [this]() {this->dir_fund_pannel();});
                    ui->stackedWidget->addWidget(chart_widget);
                    ui->stackedWidget->setCurrentWidget(chart_widget);
                });
        if (this->g_fund_manager != nullptr) {fund_pannel->g_fund_manager = this->g_fund_manager;
        } else {
            fund_pannel->g_fund_manager = new model::datas_fund_manager();
            this->g_fund_manager = fund_pannel->g_fund_manager;  // 保持唯一
            fund_pannel->g_fund_manager->fast_low_level_load_data(core::funds_row_dir);
        }
        fund_pannel->connect_funds_ui();
        ui->stackedWidget->addWidget(fund_pannel);
    }
    ui->stackedWidget->setCurrentWidget(fund_pannel);
    fund_pannel->refresh_funds_ui();
}


