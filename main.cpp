#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/sugar.png"));

    MainWindow w;
    w.show();

    return a.exec();
}
