#ifndef FUND_CARD_H
#define FUND_CARD_H
#include <model/fund_model.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class fund_card;
}
QT_END_NAMESPACE

class fund_card : public QWidget
{
    Q_OBJECT

public:
    void align_info_ui();    // 对齐数据和 ui
    QString fund_code;
    model::fund_model *the_fund_entity = nullptr;
    explicit fund_card(model::fund_model *entity = nullptr,QString fund_code="", QWidget *parent = nullptr);
    ~fund_card() override;
signals:
    void dir_chart(model::fund_model* fund_ptr);
    void buy_me(float val,QString fund_code);
    void sell_me(float ratio,QString fund_code);
private:
    Ui::fund_card *ui;

protected:
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

};


#endif // FUND_CARD_H