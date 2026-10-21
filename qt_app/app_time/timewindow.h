#ifndef TIMEWINDOW_H
#define TIMEWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class TimeWindow;
}
QT_END_NAMESPACE

class TimeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TimeWindow(QWidget *parent = nullptr);
    ~TimeWindow() override;

signals: // 【修正：关键字是 signals，而不是 signed】
    void time_updated(); // 【定义一个清晰的信号】

private:
    Ui::TimeWindow *ui;
};
#endif // TIMEWINDOW_H