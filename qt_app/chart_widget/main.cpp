#include <QApplication>
#include <QDebug>
#include "customchartview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 创建带 UI 的画布页面
    customchartview* page = new customchartview;
    page->setWindowTitle("强制落笔(0,1) + 采样导出");
    page->resize(900, 650);
    page->show();

    // 接收抬起笔后的完整结果
    QObject::connect(page, &customchartview::resultReady,
                     [](const std::vector<DataPoint>& data,
                        double similarity,
                        int sampleCount)
                     {
                         qDebug() << "========================================";
                         qDebug() << "【抬起笔】收到结果";
                         qDebug() << "采样数量设定:" << sampleCount;
                         qDebug() << "实际采样点数:" << data.size();
                         qDebug() << "similarity  :" << similarity;
                         qDebug() << "----------------------------------------";

                         for (int i = 0; i < (int)data.size(); ++i) {
                             qDebug() << QString("  [%1]  x = %2 , y = %3")
                             .arg(i, 2)
                                 .arg(data[i].x, 0, 'f', 4)
                                 .arg(data[i].y, 0, 'f', 4);
                         }
                         qDebug() << "========================================\n";
                     });

    return a.exec();
}