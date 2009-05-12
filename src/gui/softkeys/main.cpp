/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtGui>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() {}
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    QWidget *central = new QWidget(this);    
    
    QMenuBar* menuBar = new QMenuBar(this);
    menuBar->addAction("MyMenuItem1");
    this->setMenuBar(menuBar);

    QSoftKeyStack *stack = new QSoftKeyStack(central);
    QSoftKeyAction action1(central);
    action1.setText(QString("Ok"));
    action1.setRole(QSoftKeyAction::Ok);
    QSoftKeyAction action2(central);
    action2.setText(QString("Back"));
    action2.setRole(QSoftKeyAction::Back);
    QSoftKeyAction action3(central);
    action3.setText(QString("Cancel"));
    action3.setRole(QSoftKeyAction::Cancel);
    QSoftKeyAction action4(central);
    action4.setText(QString("Menu"));
    action4.setRole(QSoftKeyAction::Menu);

    QList<QSoftKeyAction*> myActionList;
    myActionList.append(&action1);
    myActionList.append(&action2);
    myActionList.append(&action3);
    stack->push(myActionList);
    stack->pop();
    stack->push(&action1);
    stack->pop();

    QList<QSoftKeyAction*> myActionList2;
    myActionList2.append(&action4);
    myActionList2.append(&action1);
    stack->push(myActionList2);
    
    
    setCentralWidget(central);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();
    return app.exec();
}

#include "main.moc"
