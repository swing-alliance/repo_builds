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
    model::fund_model *the_fund_entity = nullptr;
    explicit fund_card(model::fund_model *entity = nullptr, QWidget *parent = nullptr);
    void setVisible(bool visible) override {
        if (this->isVisible() != visible) {
            QWidget::setVisible(visible); // 保持原有的显隐行为
            emit visibility_changed(visible);
        }
    }
signals:
    void visibility_changed(bool visible);
    ~fund_card() override;

private:
    Ui::fund_card *ui;

protected:
    void paintEvent(QPaintEvent *event) override;

};

#endif // FUND_CARD_H