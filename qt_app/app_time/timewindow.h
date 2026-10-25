#ifndef TIMEWINDOW_H
#define TIMEWINDOW_H

#include <QMainWindow>
#include<virtual/v_app.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class TimeWindow;
}
QT_END_NAMESPACE

class TimeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TimeWindow(v_sim::v_sim_pack * v_pack_ptr=nullptr,QWidget *parent = nullptr);
    ~TimeWindow() override;

signals:
    void time_updated();

private:
    Ui::TimeWindow *ui;
};
#endif // TIMEWINDOW_H