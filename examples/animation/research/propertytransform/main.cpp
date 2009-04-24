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

#include <QtGui>

#include "qpropertytransform.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    
    QGraphicsItem *item = new QGraphicsRectItem(QRectF(0,0, 200, 100));
    scene.addItem(item);
    QPropertyTransform transform;
    transform.setTargetItem(item);

    QAnimationGroup *group = new QAnimationGroup(QAnimationGroup::Parallel, &scene);
    QPropertyAnimation *scaleAnim = new QPropertyAnimation(&transform, "scaleX");
    scaleAnim->setStartValue(1.0);
    scaleAnim->setTargetValue(2.0);
    scaleAnim->setDuration(10000);
    group->add(scaleAnim);

    QPropertyAnimation *scaleAnim2 = new QPropertyAnimation(&transform, "scaleY");
    scaleAnim2->setStartValue(.0);
    scaleAnim2->setTargetValue(2.0);
    scaleAnim2->setDuration(10000);
    QEasingCurve curve(QEasingCurve::InElastic);
    curve.setPeriod(2);
    curve.setAmplitude(2);

    //scaleAnim2->setEasingCurve(curve);
    //scaleAnim2->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic , 2, 2 ));
    group->add(scaleAnim2);
    
    QPropertyAnimation *rotAnim = new QPropertyAnimation(&transform, "rotation");
    rotAnim->setStartValue(0);
    rotAnim->setTargetValue(90);
    rotAnim->setDuration(10000);
    group->add(rotAnim);
    
    group->start();

    view.resize(800,600);
    view.show();
    return app.exec();
}
