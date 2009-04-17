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

#ifndef __MAINWINDOW__H__
#define __MAINWINDOW__H__

#include <QtGui>

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow();

private slots :
    void onEnterState3();
    void onEnterState2();
    void onEnterState1();
    void onItemClicked(QListWidgetItem*);
    void onRemoveClicked();
private:
    QListWidget *listWidget;
    QLabel *labelWidget;
    QLabel *label2Widget;
    QLineEdit *editWidget;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QState *state1;
    QState *state2;
    QState *state3;
    QState *currentState;
    QState *group;
    QStateMachine *machine;
};

#endif //__MAINWINDOW__H__

