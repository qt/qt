#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDeclarative>
#include <QtGui>
#include <QtCore>
#include "testmodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void createMenus();

signals:

public slots:
    void runTests();
    void runAllTests();
    void showFixScreen(const QString& path);

private:
    QDeclarativeView* view;
    TestModel *curTest;
    QStringList tests;
    int testIdx;
};

#endif // MAINWINDOW_H
