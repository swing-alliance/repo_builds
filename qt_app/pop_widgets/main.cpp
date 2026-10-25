#include "cal_param_widget.h"

#include <QApplication>
#include <model/fund_model.h>
#include "buy_transaction_widget.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    buy_transaction_widget  *w= new buy_transaction_widget(0,"dasd");
    w->show();
    return QApplication::exec();
}
