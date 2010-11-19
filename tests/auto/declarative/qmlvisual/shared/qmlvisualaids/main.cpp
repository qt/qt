#include <QtGui/QApplication>
#include <QtDeclarative>
#include "mainwindow.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *m = new MainWindow;
    m->show();
    return a.exec();
}
