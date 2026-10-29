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
#include<virtual/v_account.h>
#include<virtual/v_tracker.h>
#include<virtual/v_brokerage.h>
#include<virtual/v_app.h>
#include <model/fund_model.h>
#include "fund_pannel_widget.h"
#include "cal_param_widget.h"
#include "ui_v_pack_pannel.h"
#include "addgroup.h"
#include "delgroup.h"
#include"load_fund_group.h"
#include<QIcon>
#include <QProcess>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
EntryWindow::EntryWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EntryWindow)
{
    ui->setupUi(this);

    setWindowTitle("入口站");
    this->main_page_timer=new QTimer(this);
    this->main_page_timer->start(500);
    ui->action_v_time->setShortcut(QKeySequence("ALT+T"));
    ui->action_cal_resort->setShortcut(QKeySequence("ALT+C"));

    connect(this->main_page_timer,&QTimer::timeout,this,[this](){this->ui->t_label->setText(QString::fromStdString(core::getnow_string()));  //绑定时间到ui
        if(this->fund_pannel!=nullptr &&this->fund_pannel->v_pannel!=nullptr){this->fund_pannel->v_pannel->ui->time_label->setText(QString::fromStdString(core::getnow_string()));}});
    connect(ui->action_v_time, &QAction::triggered, this, [this]() {   //调用虚拟时间实现
        if (this->t != nullptr) {
            this->t->raise();
            this->t->activateWindow();
            return;
        }
        if(this->fund_pannel!=nullptr&&this->fund_pannel->v_sim_pack!=nullptr){this->t = new TimeWindow(this->fund_pannel->v_sim_pack,nullptr);}else{this->t = new TimeWindow(nullptr,nullptr);}
        connect(this->t, &TimeWindow::time_updated, this, [this]() {
            if (this->g_fund_manager == nullptr) {
                this->g_fund_manager->fast_low_level_load_data(core::funds_row_dir);
            }
            if (this->fund_pannel != nullptr && ui->stackedWidget->currentWidget() == this->fund_pannel) {
                this->fund_pannel->refresh_funds_ui();
            }else if (this->fund_pannel != nullptr && ui->stackedWidget->currentWidget() == this->chart_widget) {
                ChartWidget *current_chart = qobject_cast<ChartWidget*>(ui->stackedWidget->currentWidget());
                if (current_chart && current_chart->fm) {
                    std::string target_code = current_chart->fm->fund_code;
                    model::fund_model* fresh_fund_ptr = nullptr;
                    if (this->g_fund_manager != nullptr) {
                        for (auto &fm : this->g_fund_manager->funds_list) {
                            if (fm.fund_code == target_code) {
                                fresh_fund_ptr = &fm;
                                break;
                            }
                        }
                    }
                    if (fresh_fund_ptr != nullptr) {
                        ui->stackedWidget->removeWidget(this->chart_widget);
                        this->chart_widget->deleteLater();
                        this->chart_widget = new ChartWidget(
                            fresh_fund_ptr,
                            QString::fromStdString(fresh_fund_ptr->fund_name),
                            QString::fromStdString(fresh_fund_ptr->fund_code),
                            this
                            );

                        connect(this->chart_widget, &ChartWidget::pop_here, this, [this]() {
                            this->dir_fund_pannel();
                        });
                        ui->stackedWidget->addWidget(this->chart_widget);
                        ui->stackedWidget->setCurrentWidget(this->chart_widget);
                    }
                }
            }
            else {
                qDebug() << "当前不在基金面板，后台数据已更新，待切过来时自动刷新。";
            }
            if(this->fund_pannel!=nullptr && this->fund_pannel->v_sim_pack!=nullptr){this->fund_pannel->v_sim_pack->v_global_tracker.v_check_to_do(this->fund_pannel->v_sim_pack->v_account);}  //后面有股票记得加
        });
        connect(this->t, &QObject::destroyed, this, [this]() {this->t = nullptr;});this->t->show();});
    connect(ui->actionlow_load,&QAction::triggered,this,[this](){                                   //低级加载
        if(this->g_fund_manager==nullptr){this->g_fund_manager=new model::datas_fund_manager;}
        this->g_fund_manager->fast_low_level_load_data(core::funds_row_dir);});
    connect(ui->actionstd_load,&QAction::triggered,this,[this](){                                   //标准加载的按钮
        if(this->g_fund_manager==nullptr){this->g_fund_manager=new model::datas_fund_manager;}
        this->g_fund_manager->std_load_data(core::funds_row_dir);});
    connect(ui->actiondir_main,&QAction::triggered,this,[this](){this->dir_main_page();});           //导航到主的按钮
    connect(ui->actiondir_fund_pannel,&QAction::triggered,this,[this](){this->dir_fund_pannel();});  //导航到鸡控制办的按钮
    connect(ui->instant_start_btn, &QPushButton::clicked, this, [this](){   //快速开始
        if(this->g_fund_manager==nullptr){ui->actionstd_load->trigger();}
        ui->actiondir_fund_pannel->trigger();});
    connect(ui->action_cal_resort,&QAction::triggered,this,[this](){      //调用计算和排序
        auto *cal_win = new cal_param_Widget(*this->g_fund_manager, this);
        cal_win->setWindowFlags(Qt::Window);
        cal_win->setAttribute(Qt::WA_DeleteOnClose);
        connect(cal_win, &cal_param_Widget::param_accepted, this, [this]() {
            if(this->fund_pannel!=nullptr){
                emit this->fund_pannel->need_refresh_cards_entity();
            }
            if(ui->stackedWidget->currentWidget()==this->fund_pannel){this->fund_pannel->refresh_funds_ui();}
        });
        cal_win->show();});
    connect(ui->actionstart_v_pack,&QAction::triggered,this,[this](){if(this->fund_pannel!=nullptr){   //初始化虚拟包，之后才能够虚拟买和卖
        emit fund_pannel->init_fund_v_pack();}});
    //======测试界=====


    //======测试界=====


}

EntryWindow::~EntryWindow(){delete ui;}

void EntryWindow::dir_main_page(){ui->stackedWidget->setCurrentIndex(0);}

void EntryWindow::dir_fund_pannel()     //导航到鸡
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


void EntryWindow::on_actionaddgroup_triggered()
{addgroup * ad_gp=new addgroup;ad_gp->show();}


void EntryWindow::on_actiondelgroup_triggered()
{delgroup * d_gp=new delgroup;d_gp->show();}


void EntryWindow::on_actionloadfromgroup_triggered()
{
    load_fund_group * ldfdg = new load_fund_group(this);
    connect(ldfdg,&load_fund_group::dir_grouped_pannel,this,[this](QString g_name){
        if (grouped_fund_pannel != nullptr)
        {
            ui->stackedWidget->removeWidget(grouped_fund_pannel);
            grouped_fund_pannel->deleteLater();
            grouped_fund_pannel = nullptr;
        }
        grouped_fund_pannel = new fund_pannel_widget(this,g_name);
        connect(grouped_fund_pannel, &fund_pannel_widget::on_card_dir_chart,this, [this](model::fund_model* fund_ptr) {
            if (!fund_ptr) return;
            if (chart_widget) {
                ui->stackedWidget->removeWidget(chart_widget);
                chart_widget->deleteLater();
                chart_widget = nullptr;
            }
            chart_widget = new ChartWidget(fund_ptr,
                                           QString::fromStdString(fund_ptr->fund_name),
                                           QString::fromStdString(fund_ptr->fund_code),
                                           this);
            connect(chart_widget, &ChartWidget::pop_here,this, [this]() {this->dir_fund_pannel();});
            ui->stackedWidget->addWidget(chart_widget);
            ui->stackedWidget->setCurrentWidget(chart_widget);
        });
        grouped_fund_pannel->g_fund_manager->std_load_data(core::datasource+'/'+"temp_group_fund");
        grouped_fund_pannel->connect_funds_ui();
        ui->stackedWidget->addWidget(grouped_fund_pannel);
        ui->stackedWidget->setCurrentWidget(grouped_fund_pannel);
        grouped_fund_pannel->refresh_funds_ui();
    });
    ldfdg->setWindowFlags(Qt::Dialog | Qt::Window);
    QEventLoop loop;
    connect(ldfdg, &QWidget::destroyed, &loop, &QEventLoop::quit);
    ldfdg->show();
    loop.exec();


}



void EntryWindow::on_actionupdate_basic_data_triggered()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString exePath = QDir(appDir).filePath("cpp_client.exe");

    // 分离启动，和双击一样，直接弹出，互不依赖
    bool ok = QProcess::startDetached(exePath);
    if (!ok)
    {
        QMessageBox::warning(this, "启动失败", "无法启动：" + exePath);
    }
}

