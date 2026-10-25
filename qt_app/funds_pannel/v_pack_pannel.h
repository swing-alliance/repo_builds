#ifndef V_PACK_PANNEL_H
#define V_PACK_PANNEL_H
#include <QWidget>
#include<virtual/v_app.h>
#include <QTimer>
namespace Ui {
class v_pack_pannel;
}

class v_pack_pannel : public QWidget
{
    Q_OBJECT

public:
    explicit v_pack_pannel(v_sim::v_sim_pack* sim_pack,QWidget *parent = nullptr);
    ~v_pack_pannel();
    QTimer *timer = new QTimer(this);
    Ui::v_pack_pannel *ui;

private:

};

#endif // V_PACK_PANNEL_H
