#ifndef BUY_TRANSACTION_WIDGET_H
#define BUY_TRANSACTION_WIDGET_H

#include <QWidget>

namespace Ui {
class buy_transaction_widget;
}

class buy_transaction_widget : public QWidget
{
    Q_OBJECT

public:
    explicit buy_transaction_widget(int buy_type_index,QString buy_code,QWidget *parent = nullptr);
    ~buy_transaction_widget();
signals:
    void buy_val(float val);
private:
    Ui::buy_transaction_widget *ui;
};

#endif // BUY_TRANSACTION_WIDGET_H
