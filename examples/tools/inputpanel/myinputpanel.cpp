/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "myinputpanel.h"

//! [0]

MyInputPanel::MyInputPanel()
    : QWidget(0, Qt::Tool | Qt::WindowStaysOnTopHint),
      lastFocusedWidget(0)
{
    form.setupUi(this);

    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
            this, SLOT(saveFocusWidget(QWidget *, QWidget *)));

    signalMapper.setMapping(form.panelButton_1, form.panelButton_1);
    signalMapper.setMapping(form.panelButton_2, form.panelButton_2);
    signalMapper.setMapping(form.panelButton_3, form.panelButton_3);
    signalMapper.setMapping(form.panelButton_4, form.panelButton_4);
    signalMapper.setMapping(form.panelButton_5, form.panelButton_5);
    signalMapper.setMapping(form.panelButton_6, form.panelButton_6);
    signalMapper.setMapping(form.panelButton_7, form.panelButton_7);
    signalMapper.setMapping(form.panelButton_8, form.panelButton_8);
    signalMapper.setMapping(form.panelButton_9, form.panelButton_9);
    signalMapper.setMapping(form.panelButton_star, form.panelButton_star);
    signalMapper.setMapping(form.panelButton_0, form.panelButton_0);
    signalMapper.setMapping(form.panelButton_hash, form.panelButton_hash);

    connect(form.panelButton_1, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_2, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_3, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_4, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_5, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_6, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_7, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_8, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_9, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_star, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_0, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_hash, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));

    connect(&signalMapper, SIGNAL(mapped(QWidget *)),
            this, SLOT(buttonClicked(QWidget *)));
}

//! [0]

bool MyInputPanel::event(QEvent *e)
{
    switch (e->type()) {
//! [1]
    case QEvent::WindowActivate:
        if (lastFocusedWidget)
            lastFocusedWidget->activateWindow();
        break;
//! [1]
    default:
        break;
    }

    return QWidget::event(e);
}

//! [2]

void MyInputPanel::saveFocusWidget(QWidget * /*oldFocus*/, QWidget *newFocus)
{
    if (newFocus != 0 && !this->isAncestorOf(newFocus)) {
        lastFocusedWidget = newFocus;
    }
}

//! [2]

//! [3]

void MyInputPanel::buttonClicked(QWidget *w)
{
    QChar chr = qvariant_cast<QChar>(w->property("buttonValue"));
    emit characterGenerated(chr);
}

//! [3]
