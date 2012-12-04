/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "circlewidget.h"
#include "window.h"

//! [0]
Window::Window()
{
    aliasedLabel = createLabel(tr("Aliased"));
    antialiasedLabel = createLabel(tr("Antialiased"));
    intLabel = createLabel(tr("Int"));
    floatLabel = createLabel(tr("Float"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(aliasedLabel, 0, 1);
    layout->addWidget(antialiasedLabel, 0, 2);
    layout->addWidget(intLabel, 1, 0);
    layout->addWidget(floatLabel, 2, 0);
//! [0]

//! [1]
    QTimer *timer = new QTimer(this);

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            circleWidgets[i][j] = new CircleWidget;
            circleWidgets[i][j]->setAntialiased(j != 0);
            circleWidgets[i][j]->setFloatBased(i != 0);

            connect(timer, SIGNAL(timeout()),
                    circleWidgets[i][j], SLOT(nextAnimationFrame()));

            layout->addWidget(circleWidgets[i][j], i + 1, j + 1);
        }
    }
//! [1] //! [2]
    timer->start(100);
    setLayout(layout);

    setWindowTitle(tr("Concentric Circles"));
}
//! [2]

//! [3]
QLabel *Window::createLabel(const QString &text)
{
    QLabel *label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setMargin(2);
    label->setFrameStyle(QFrame::Box | QFrame::Sunken);
    return label;
}
//! [3]
