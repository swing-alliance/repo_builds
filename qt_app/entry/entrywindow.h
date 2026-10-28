#ifndef ENTRYWINDOW_H
#define ENTRYWINDOW_H

#include "model/fund_model.h"
#include <QMainWindow>
#include <QTimer>
#include"chartwidget.h"

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

signals:
    // 真正需要对外发出的信号才放这里
    // void dir_chart_here(model::fund_model* fund_ptr);  // 如果需要的话

private slots:
    void dir_main_page();
    void dir_fund_pannel();


    void on_actionaddgroup_triggered();

    void on_actiondelgroup_triggered();

private:
    ChartWidget* chart_widget = nullptr;
    TimeWindow * t = nullptr;
    QTimer * main_page_timer = nullptr;
    Ui::EntryWindow *ui;
    model::datas_fund_manager * g_fund_manager = nullptr;   //创建了不可以删除
    fund_pannel_widget * fund_pannel = nullptr;

};
#endif // ENTRYWINDOW_H
