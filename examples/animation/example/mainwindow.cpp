/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"

MainWindow::MainWindow() : QMainWindow(0)
{
    // Text edit and button
    listWidget = new QListWidget;
    new QListWidgetItem("Rachel", listWidget);
    new QListWidgetItem("Andreas", listWidget);
    new QListWidgetItem("David", listWidget);
    new QListWidgetItem("Olivier", listWidget);
    new QListWidgetItem("Andy", listWidget);
    new QListWidgetItem("Martial", listWidget);
    new QListWidgetItem("Kazou", listWidget);
    new QListWidgetItem("Fred", listWidget);
    new QListWidgetItem("Ingrid", listWidget);
    QGraphicsProxyWidget *listProxy = new QGraphicsProxyWidget;
    listProxy->setWidget(listWidget);

    labelWidget = new QLabel;
    QGraphicsProxyWidget *labelProxy = new QGraphicsProxyWidget;
    labelProxy->setWidget(labelWidget);
    labelWidget->setAttribute(Qt::WA_NoSystemBackground);

    label2Widget = new QLabel;
    label2Widget->setAlignment(Qt::AlignCenter);
    QGraphicsProxyWidget *label2Proxy = new QGraphicsProxyWidget;
    label2Proxy->setWidget(label2Widget);
    label2Widget->setAttribute(Qt::WA_NoSystemBackground);

    editWidget = new QLineEdit;
    QGraphicsProxyWidget *editProxy = new QGraphicsProxyWidget;
    editProxy->setWidget(editWidget);
    editWidget->setAttribute(Qt::WA_NoSystemBackground);

    // Parent widget
    QGraphicsWidget *widget = new QGraphicsWidget;
    // Parent widget
    QGraphicsWidget *widget2 = new QGraphicsWidget;

    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout(Qt::Vertical, widget);
    vLayout->addItem(listProxy);
    vLayout->addItem(widget2);
    widget->setLayout(vLayout);

    QPushButton *button = new QPushButton;
    QGraphicsProxyWidget *buttonProxy = new QGraphicsProxyWidget;
    buttonProxy->setWidget(button);

    QPushButton *button2 = new QPushButton;
    QGraphicsProxyWidget *buttonProxy2 = new QGraphicsProxyWidget;
    buttonProxy2->setWidget(button2);

    QPushButton *button3 = new QPushButton;
    QGraphicsProxyWidget *buttonProxy3 = new QGraphicsProxyWidget;
    buttonProxy3->setWidget(button3);

    QPushButton *button4 = new QPushButton;
    QGraphicsProxyWidget *buttonProxy4 = new QGraphicsProxyWidget;
    buttonProxy4->setWidget(button4);

    QGraphicsLinearLayout *hLayout = new QGraphicsLinearLayout(Qt::Horizontal, widget2);
    hLayout->addItem(buttonProxy);
    hLayout->addItem(buttonProxy2);
    hLayout->addItem(buttonProxy3);
    widget2->setLayout(hLayout);

    scene = new QGraphicsScene(0, 0, 700, 600);
    scene->setBackgroundBrush(scene->palette().window());
    scene->addItem(widget);
    scene->addItem(editProxy);
    scene->addItem(label2Proxy);
    scene->addItem(labelProxy);
    scene->addItem(buttonProxy4);

    machine = new QStateMachine();

    group = new QState(machine->rootState());
    state1 = new QState(group);
    state2 = new QState(group);
    state3 = new QState(group);
    group->setInitialState(state1);

    machine->setInitialState(group);

    // State 1
    state1->assignProperty(button, "text", "Edit");
    state1->assignProperty(button2, "text", "Add");
    state1->assignProperty(button3, "text", "Remove");
    state1->assignProperty(button4, "text", "Accept");
    state1->addTransition(button2, SIGNAL(clicked()), state3);
    state1->assignProperty(listProxy, "geometry", QRectF(0, 0, 700, 560));
    state1->assignProperty(widget, "geometry", QRectF(0, 0, 700, 600));
    state1->assignProperty(editProxy, "opacity", double(0));
    state1->assignProperty(labelProxy, "opacity", double(0));
    state1->assignProperty(label2Proxy, "opacity", double(0));
    state1->assignProperty(buttonProxy4, "opacity", double(0));
    state1->assignProperty(labelWidget, "text", "Name : ");
    state1->assignProperty(label2Widget, "text", "Edit a contact");
    state1->assignProperty(label2Proxy, "geometry", QRectF(375, -50, 300, 30));
    state1->assignProperty(labelProxy, "geometry", QRectF(350, 300, 100, 30));
    state1->assignProperty(editProxy, "geometry", QRectF(750, 300, 250, 30));
    state1->assignProperty(buttonProxy4, "geometry", QRectF(500, 350, 80, 25));

    // State 2
    state2->assignProperty(button, "text", "Close Editing");
    state2->assignProperty(listProxy, "geometry", QRectF(0, 0, 350, 560));
    state2->addTransition(button2, SIGNAL(clicked()), state3);
    state2->addTransition(button4, SIGNAL(clicked()), state1);

    state2->assignProperty(editProxy, "opacity", double(1));
    state2->assignProperty(labelProxy, "opacity", double(1));
    state2->assignProperty(label2Proxy, "opacity", double(1));
    state2->assignProperty(buttonProxy4, "opacity", double(1));

    state2->assignProperty(label2Proxy, "geometry", QRectF(375, 250, 300, 30));
    state2->assignProperty(editProxy, "geometry", QRectF(440, 300, 260, 30));

     // State 3
    state3->assignProperty(button4, "text", "Create New");
    state3->assignProperty(listProxy, "geometry", QRectF(0, 0, 350, 560));
    state3->addTransition(button4, SIGNAL(clicked()), state1);
    state3->addTransition(button, SIGNAL(clicked()), state1);
    state3->assignProperty(editProxy, "opacity", double(1));
    state3->assignProperty(labelProxy, "opacity", double(1));
    state3->assignProperty(label2Proxy, "opacity", double(1));
    state3->assignProperty(buttonProxy4, "opacity", double(1));

    state3->assignProperty(label2Proxy, "geometry", QRectF(375, 250, 300, 30));
    state3->assignProperty(editProxy, "geometry", QRectF(440, 300, 260, 30));

    {
      QAnimationGroup *animationGroup = new QParallelAnimationGroup;
      QVariantAnimation *anim = new QPropertyAnimation(labelProxy, "opacity");
      animationGroup->addAnimation(anim);
      anim = new QPropertyAnimation(label2Proxy, "geometry");
      animationGroup->addAnimation(anim);
      anim = new QPropertyAnimation(editProxy, "geometry");
      animationGroup->addAnimation(anim);
      anim = new QPropertyAnimation(listProxy, "geometry");
      animationGroup->addAnimation(anim);
      
      QAbstractTransition *trans = state1->addTransition(button, SIGNAL(clicked()), state2);
      trans->addAnimation(animationGroup);
    }

    {
        QVariantAnimation *anim;
        QAnimationGroup *animationGroup = new QParallelAnimationGroup;
        anim = new QPropertyAnimation(label2Proxy, "geometry");
        animationGroup->addAnimation(anim);
        anim = new QPropertyAnimation(editProxy, "geometry");
        animationGroup->addAnimation(anim);
        anim = new QPropertyAnimation(listProxy, "geometry");
        animationGroup->addAnimation(anim);
        QAbstractTransition *trans = state2->addTransition(button, SIGNAL(clicked()), state1);
        trans->addAnimation(animationGroup);
    }

    currentState = state1;

    view = new QGraphicsView(scene);

    setCentralWidget(view);

    QObject::connect(state3, SIGNAL(entered()), this, SLOT(onEnterState3()));
    QObject::connect(state2, SIGNAL(entered()), this, SLOT(onEnterState2()));
    QObject::connect(state1, SIGNAL(entered()), this, SLOT(onEnterState1()));

    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClicked(QListWidgetItem*)));
    connect(button3, SIGNAL(clicked()), this, SLOT(onRemoveClicked()));

    machine->start();
}

void MainWindow::onEnterState2()
{
    currentState = state2;
    if (listWidget->currentItem())
        editWidget->setText(listWidget->currentItem()->text());
}

void MainWindow::onEnterState1()
{
    if (currentState == state2 && listWidget->currentItem())
        listWidget->currentItem()->setText(editWidget->text());
    if (currentState == state3 && !editWidget->text().isNull()) {
        new QListWidgetItem(editWidget->text(), listWidget);
        editWidget->clear();
    }
    currentState = state1;
}

void MainWindow::onEnterState3()
{
    currentState = state3;
}

void MainWindow::onItemClicked(QListWidgetItem*)
{
    if (currentState == state2)
    {
        editWidget->setText(listWidget->currentItem()->text());
        editWidget->clear();
    }
}

void MainWindow::onRemoveClicked()
{
    QListWidgetItem *listItem = listWidget->takeItem(listWidget->currentRow());
    delete listItem;
}
