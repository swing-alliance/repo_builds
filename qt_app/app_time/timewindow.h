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

private:
    Ui::TimeWindow *ui;
};
#endif // TIMEWINDOW_H
