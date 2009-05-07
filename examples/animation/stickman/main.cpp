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

#include "animation.h"
#include "node.h"
#include "lifecycle.h"
#include "stickman.h"
#include "graphicsview.h"

#include <QtCore>
#include <QtGui>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    StickMan *stickMan = new StickMan;
    stickMan->setDrawSticks(false);

    QGraphicsTextItem *textItem = new QGraphicsTextItem();
    textItem->setHtml("<font color=\"white\"><b>Stickman</b>"
        "<p>"
        "Tell the stickman what to do!"
        "</p>"
        "<p><i>"
        "<li>Press <font color=\"purple\">J</font> to make the stickman jump.</li>"
        "<li>Press <font color=\"purple\">D</font> to make the stickman dance.</li>"
        "<li>Press <font color=\"purple\">C</font> to make him chill out.</li>"
        "<li>When you are done, press <font color=\"purple\">Escape</font>.</li>"
        "</i></p>"
        "<p>If he is unlucky, the stickman will get struck by lightning, and never jump, dance or chill out again."
        "</p></font>");
    qreal w = textItem->boundingRect().width();
    QRectF stickManBoundingRect = stickMan->mapToScene(stickMan->boundingRect()).boundingRect();
    textItem->setPos(-w / 2.0, stickManBoundingRect.bottom() + 25.0);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->addItem(stickMan);
    scene->addItem(textItem);
    scene->setBackgroundBrush(Qt::black);

    GraphicsView *view = new GraphicsView();
    view->setRenderHints(QPainter::Antialiasing);
    view->setTransformationAnchor(QGraphicsView::NoAnchor);
    view->setScene(scene);
    view->showFullScreen();
    view->setFocus();    
    view->setSceneRect(scene->sceneRect());
    
    LifeCycle *cycle = new LifeCycle(stickMan, view);
    cycle->setDeathAnimation("animations/dead");
    
    cycle->addActivity("animations/jumping", Qt::Key_J);
    cycle->addActivity("animations/dancing", Qt::Key_D);
    cycle->addActivity("animations/chilling", Qt::Key_C);
    cycle->start();

    return app.exec();
}
