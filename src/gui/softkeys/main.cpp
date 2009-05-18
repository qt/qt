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
    ~MainWindow();
    QMenu *contextMenu;
    QAction* context1;
    QAction* context2;
    QAction* context3;
    QSoftKeyAction *action1;
    QSoftKeyAction *action2;
    QSoftKeyAction *action3;
    QSoftKeyAction *action4;
    QSoftKeyAction *action5;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      contextMenu(0),
      context1(0), context2(0), context3(0),
      action1(0),action2(0),action3(0),action4(0),action5(0)
      
{
    QWidget *central = new QWidget(this);
    contextMenu = new QMenu();

    central->setLayout(new QVBoxLayout);
    central->layout()->addWidget(new QPushButton);
    central->layout()->addWidget(new QPushButton);
    central->layout()->addWidget(new QPushButton);
    context1 = new QAction(QString("Context1"), central);
    context2 = new QAction(QString("Context2"), central);

    context3 = new QAction(QString("Context3"), contextMenu);
    central->addAction(context1);
    central->addAction(context2);
    QMenuBar* menuBar = new QMenuBar(this);
    menuBar->addAction("MyMenuItem1");
    this->setMenuBar(menuBar);
    context2->setMenu(contextMenu);
    contextMenu->addAction(context3);
    
    action1 = new QSoftKeyAction(QSoftKeyAction::Ok, QString("Ok"), central);
    action2 = new QSoftKeyAction(QSoftKeyAction::Back, QString("Back"), central);
    action3 = new QSoftKeyAction(QSoftKeyAction::Cancel, QString("Cancel"), central);
    action4 = new QSoftKeyAction(QSoftKeyAction::Menu, QString("Menu"), central);
    action5 = new QSoftKeyAction(QSoftKeyAction::ContextMenu,QString("ContextMenu"), central);
    
   
    QList<QSoftKeyAction*> myActionList;
    myActionList.append(action1);
    myActionList.append(action2);
    myActionList.append(action3);
    softKeyStack()->push(myActionList);
    softKeyStack()->pop();
    softKeyStack()->push(action1);
    softKeyStack()->pop();

    QList<QSoftKeyAction*> myActionList2;
    myActionList2.append(action4);
    myActionList2.append(action5);
    softKeyStack()->push(myActionList2);
    
    
    setCentralWidget(central);
}
MainWindow::~MainWindow()
{
    delete context1;
    delete context2;
    delete context3;
    delete action1;
    delete action2;
    delete action3;
    delete action4;
    delete action5;
    delete contextMenu;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();
    return app.exec();
}

#include "main.moc"
