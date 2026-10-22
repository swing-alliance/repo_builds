// #include <QApplication>
// #include <vector>
// #include <utility>
// #include <string>
// #include <QString>
// #include <cstdlib>   // 包含 rand() 和 srand()
// #include <ctime>     // 包含 time()
// #include "simpleplotwidget.h"

// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);

//     // 初始化随机数种子
//     std::srand(static_cast<unsigned int>(std::time(nullptr)));

//     // 1. 准备 100 个模拟数据：X 轴为 QString（日期），Y 轴为 double（净值）
//     std::vector<std::pair<QString, double>> fundData;
//     fundData.reserve(100);

//     double baseValue = 10.0; // 起始净值
//     for (int i = 1; i <= 100; ++i) {
//         // 模拟每天净值上下浮动 (-0.3 到 +0.3 之间)
//         double randomChange = (std::rand() % 61 - 30) / 100.0;
//         baseValue += randomChange;

//         // 确保净值不小于 1.0
//         if (baseValue < 1.0) baseValue = 1.0;

//         // 生成日期字符串，如 "Day 1", "Day 2" 或 "1月1日" 等
//         QString dateStr = QString("3-%1").arg(i); // 也可以用简单编号

//         fundData.push_back({dateStr, baseValue});
//     }

//     // 2. 定义坐标轴的描述文本 (std::string)
//     std::string xTitle = "(100天模拟)";
//     std::string yTitle = "(Net Value)";

//     // 3. 实例化组件：X 轴是 QString，Y 轴是 double
//     SimplePlotWidget<QString, double> *plotWidget =
//         new SimplePlotWidget<QString, double>(fundData, xTitle, yTitle);

//     // 4. 设置窗口属性并显示
//     plotWidget->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动释放内存
//     plotWidget->resize(900, 500); // 稍微调大窗口，以便看得清 100 个点的密集走势
//     plotWidget->setWindowTitle("100个模拟数据基金走势图（支持滚轮缩放与拖拽）");
//     plotWidget->show();

//     return a.exec();
// }
#include <QApplication>
#include "chartwidget.h" // 替换为你自己的主窗口头文件

int main(int argc, char *argv[])
{
    qDebug()<<"what";
    return 0;
}