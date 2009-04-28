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
#include "qvalueanimation.h"

AbstractProperty *qGraphicsItemProperty(QGraphicsItem *item, const char *property)
{
    if (qstrcmp(property, "pos") == 0) {
        return new MemberFunctionProperty<QGraphicsItem, QPointF>(item, &QGraphicsItem::pos, &QGraphicsItem::setPos);
    }
    return 0;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGraphicsScene scene;
    QGraphicsView view(&scene);

    QGraphicsItem *item = new QGraphicsRectItem(QRectF(0,0, 200, 100));
    scene.addItem(item);

    QValueAnimation *posAnim = new QValueAnimation;
    posAnim->setStartValue(QPointF(0,0));
    posAnim->setEndValue(QPointF(400, 0));
    posAnim->setDuration(1000);
    // Alternative 1
    //posAnim->setMemberFunction(item, &QGraphicsItem::pos, &QGraphicsItem::setPos);

    // Alternative 2
    //posAnim->setProperty(qMemberFunctionProperty(item, &QGraphicsItem::pos, &QGraphicsItem::setPos));
    
    // Alternative 3
    posAnim->setProperty(qGraphicsItemProperty(item, "pos"));
    
    // Alternative 4, (by implementing the qGraphicsItemProperty  QGraphicsItem::property())
    //posAnim->setProperty(item->property("pos"));

    // can also do this, which abstracts away the whole property thing.
    // i.e. this interface can also be used for QObject-properties:
    //posAnim->setAnimationProperty(animationProperty);

    posAnim->start();

    view.resize(800,600);
    view.show();
    return app.exec();
}

