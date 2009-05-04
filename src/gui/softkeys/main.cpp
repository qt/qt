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

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include "qsoftkeystack.h"

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
    QVBoxLayout *layout = new QVBoxLayout(central);

    QPushButton *button = new QPushButton("Hello");
    layout->addWidget(button);

    QSoftKeyStack *stack = new QSoftKeyStack(central);
    QSoftKeyAction action1(central);
    action1.setText(QString("text1"));
    action1.setRole(QSoftKeyAction::Ok);
    QSoftKeyAction action2(central);
    action2.setText(QString("text2"));
    action2.setRole(QSoftKeyAction::Back);
    QSoftKeyAction action3(central);
    action3.setText(QString("text3"));
    action3.setRole(QSoftKeyAction::Cancel);

    QList<QSoftKeyAction*> myActionList;
    myActionList.append(&action1);
    myActionList.append(&action2);
    myActionList.append(&action3);
    stack->push(myActionList);
    stack->pop();
    stack->push(&action1);
    
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
