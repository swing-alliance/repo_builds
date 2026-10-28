#include "entrywindow.h"
#include<QIcon>
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EntryWindow  w;
    w.setWindowIcon(QIcon(":/infinite.png"));


    w.show();
    return QApplication::exec();
}
