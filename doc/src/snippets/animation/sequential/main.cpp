/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QApplication>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include "tracer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.resize(720, 96);
    window.show();

    QLabel *label1 = new QLabel(&window);
    label1->setPixmap(QPixmap(":/icons/left.png"));
    label1->move(16, 16);
    label1->show();

    QLabel *label2 = new QLabel(&window);
    label2->setPixmap(QPixmap(":/icons/right.png"));
    label2->move(320, 16);
    label2->show();

    QPropertyAnimation *anim1 = new QPropertyAnimation(label1, "pos");
    anim1->setDuration(2500);
    anim1->setStartValue(QPoint(16, 16));
    anim1->setEndValue(QPoint(320, 16));

    QPropertyAnimation *anim2 = new QPropertyAnimation(label2, "pos");
    anim2->setDuration(2500);
    anim2->setStartValue(QPoint(320, 16));
    anim2->setEndValue(QPoint(640, 16));

    QSequentialAnimationGroup group;
    group.addAnimation(anim1);
    group.addAnimation(anim2);

    Tracer tracer(&window);

    QObject::connect(anim1, SIGNAL(valueChanged(QVariant)),
                     &tracer, SLOT(recordValue(QVariant)));
    QObject::connect(anim2, SIGNAL(valueChanged(QVariant)),
                     &tracer, SLOT(recordValue(QVariant)));
    QObject::connect(anim1, SIGNAL(finished()), &tracer, SLOT(checkValue()));
    QObject::connect(anim2, SIGNAL(finished()), &tracer, SLOT(checkValue()));

    group.start();
    return app.exec();
}
