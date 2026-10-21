#ifndef ENTRYWINDOW_H
#define ENTRYWINDOW_H

#include "model/fund_model.h"
#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class EntryWindow;

}
QT_END_NAMESPACE
//=======================声明==============================
class TimeWindow;
class test_login; //测试用
class fund_pannel_widget;
namespace model {
class datas_fund_manager;
}
class EntryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EntryWindow(QWidget *parent = nullptr);
    ~EntryWindow() override;

private slots:
    void dir_main_page();
    void dir_fund_pannel();
    //void connect_fund_manager_ui(); //把如fund_manger连接到ui

private:
    TimeWindow * t;  //弹出来的ui

    QTimer * main_page_timer=nullptr;             //非ui
    Ui::EntryWindow *ui;          //主窗口ui
    model::datas_fund_manager * g_fund_manager=nullptr;    //全局唯一
    //=================添加再stackwidget上的======================
    test_login *loginPage = nullptr; //测试用
    fund_pannel_widget * fund_pannel=nullptr;
    //=========================界================================
};
#endif // ENTRYWINDOW_H
