#ifndef SELL_TRANSACTION_WIDGET_H
#define SELL_TRANSACTION_WIDGET_H

#include <QWidget>

namespace Ui {
class sell_transaction_widget;
}

class sell_transaction_widget : public QWidget
{
    Q_OBJECT

public:
    explicit sell_transaction_widget(int sell_type_index,QString sell_code,QWidget *parent = nullptr);
    ~sell_transaction_widget();
signals:
    void sell_ratio(float ratio);

private:
    Ui::sell_transaction_widget *ui;
};

#endif // SELL_TRANSACTION_WIDGET_H
