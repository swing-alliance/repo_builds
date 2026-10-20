#ifndef ENTRYWINDOW_H
#define ENTRYWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class EntryWindow;

}
QT_END_NAMESPACE
class TimeWindow;
class test_login; //测试用
class EntryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EntryWindow(QWidget *parent = nullptr);
    ~EntryWindow() override;

private slots:
    void dir_main_page();

private:
    TimeWindow * t;  //弹出来的ui

    QTimer *timer;  //非ui

    Ui::EntryWindow *ui;  //主窗口ui
    test_login *loginPage = nullptr; //测试用
};
#endif // ENTRYWINDOW_H
