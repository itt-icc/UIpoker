#include "MainFrame.h"
#include <QApplication>
#include <QTranslator>
#include <QFile>

int main(int argc, char *argv[])
{
    /*开始运行程序*/
    QApplication a(argc, argv);
    /*创建窗口*/
    MainFrame w;
    w.setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    /*主窗口显示*/
    w.show();
    return a.exec();
}
