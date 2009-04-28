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

#include "panel.h"
#include "roundrectitem.h"
#include "splashitem.h"
#include "ui_backside.h"

#ifndef QT_NO_OPENGL
#include <QtOpenGL/QtOpenGL>
#else
#endif
#include <QtGui/QtGui>

Panel::Panel(int width, int height)
    : selectedIndex(0),
      grid(width*height),
      width(width),
      height(height),
      flipped(false),
      flippingGroup(0),
      rotationXanim(0),
      rotationYanim(0)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(FullViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setBackgroundBrush(QPixmap(":/images/blue_angle_swirl.jpg"));
#ifndef QT_NO_OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

    QRectF bounds((-width / 2.0) * 150, (-height / 2.0) * 150, width * 150, height * 150);

    setScene(new QGraphicsScene(bounds, this));

    baseItem = new RoundRectItem(bounds, QColor(226, 255, 92, 64));
    scene()->addItem(baseItem);

    QWidget *embed = new QWidget;
    ui = new Ui_BackSide;
    ui->setupUi(embed);
    ui->hostName->setFocus();

    backItem = new RoundRectItem(bounds, embed->palette().window(), embed);
    backItem->setYRotation(180);
    backItem->setParentItem(baseItem);
        
    selectionItem = new RoundRectItem(QRectF(-60, -60, 120, 120), Qt::gray);
    selectionItem->setParentItem(baseItem);
    selectionItem->setZValue(-1);
    selectionItem->setPos(posForLocation(0));

    int currentIndex = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            RoundRectItem *item = new RoundRectItem(QRectF(-54, -54, 108, 108),
                                                    QColor(214, 240, 110, 128));
            item->setPos(posForLocation(currentIndex));
                
            item->setParentItem(baseItem);
            item->setFlag(QGraphicsItem::ItemIsFocusable);
            grid[currentIndex++] = item;

            switch (qrand() % 9) {
            case 0: item->setPixmap(QPixmap(":/images/kontact_contacts.png")); break;
            case 1: item->setPixmap(QPixmap(":/images/kontact_journal.png")); break;
            case 2: item->setPixmap(QPixmap(":/images/kontact_notes.png")); break;
            case 3: item->setPixmap(QPixmap(":/images/kopeteavailable.png")); break;
            case 4: item->setPixmap(QPixmap(":/images/metacontact_online.png")); break;
            case 5: item->setPixmap(QPixmap(":/images/minitools.png")); break;
            case 6: item->setPixmap(QPixmap(":/images/kontact_journal.png")); break;
            case 7: item->setPixmap(QPixmap(":/images/kontact_contacts.png")); break;
            case 8: item->setPixmap(QPixmap(":/images/kopeteavailable.png")); break;
            default:
                break;
            }

            connect(item, SIGNAL(activated()), this, SLOT(flip()));
        }
    }

    grid.first()->setFocus();

    connect(backItem, SIGNAL(activated()),
            this, SLOT(flip()));

    splash = new SplashItem;
    splash->setZValue(5);
    splash->setPos(-splash->rect().width() / 2, scene()->sceneRect().top());
    scene()->addItem(splash);

    splash->grabKeyboard();
    
    //initialize the position
    baseItem->setYRotation(selectionItem->x()/6.);
    baseItem->setXRotation(selectionItem->y()/6.);

    setWindowTitle(tr("Pad Navigator Example"));
}

Panel::~Panel()
{
}

void Panel::keyPressEvent(QKeyEvent *event)
{
    if (splash->isVisible() || event->key() == Qt::Key_Return || flipped) {
        QGraphicsView::keyPressEvent(event);
        return;
    }

    selectedIndex = (selectedIndex + grid.count() + (event->key() == Qt::Key_Right) - (event->key() == Qt::Key_Left)
         + width * ((event->key() == Qt::Key_Down) - (event->key() == Qt::Key_Up))) % grid.count();
    grid[selectedIndex]->setFocus();

    const QPointF pos = posForLocation(selectedIndex);

    const double angleY = pos.x() / 6.,
              angleX = pos.y() / 6.;

    QAnimationGroup *group = new QParallelAnimationGroup();

    QVariantAnimation *anim = new QPropertyAnimation(baseItem, "xRotation");
    anim->setEndValue(angleX);
    anim->setDuration(150);
    anim->setEasingCurve(QEasingCurve::OutInSine);
    group->addAnimation(anim);

    anim = new QPropertyAnimation(baseItem, "yRotation");
    anim->setEndValue(angleY);
    anim->setDuration(150);
    anim->setEasingCurve(QEasingCurve::OutInSine);
    group->addAnimation(anim);

    anim = new QPropertyAnimation(selectionItem, "pos");
    anim->setEndValue(pos);
    anim->setDuration(150);
    anim->setEasingCurve(QEasingCurve::Linear);
    group->addAnimation(anim);

    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void Panel::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void Panel::flip()
{
    grid[selectedIndex]->setFocus();

    if (flippingGroup == 0) {
        flippingGroup = new QParallelAnimationGroup(this);

        const qreal zoomOut = qreal(.75);

        //slight scaling down while flipping
        QVariantAnimation *anim = new QPropertyAnimation(baseItem, "yScale");
        anim->setKeyValueAt(qreal(.5), zoomOut);
        anim->setEndValue(1);
        anim->setEasingCurve(QEasingCurve::OutInSine);
        anim->setDuration(500);
        flippingGroup->addAnimation(anim);

        anim = new QPropertyAnimation(baseItem, "xScale");
        anim->setKeyValueAt(qreal(.5), zoomOut);
        anim->setEndValue(1);
        anim->setEasingCurve(QEasingCurve::OutInSine);
        anim->setDuration(500);
        flippingGroup->addAnimation(anim);

        rotationXanim = new QPropertyAnimation(baseItem, "xRotation");
        rotationXanim->setEndValue(0);
        rotationXanim->setDuration(500);
        flippingGroup->addAnimation(rotationXanim);

        rotationYanim = new QPropertyAnimation(baseItem, "yRotation");
        rotationYanim->setEndValue(180);
        rotationYanim->setDuration(500);
        flippingGroup->addAnimation(rotationYanim);
    }

    if (flippingGroup->currentTime() != 0 && flippingGroup->direction() == QAbstractAnimation::Forward) {
        flippingGroup->setDirection(QAbstractAnimation::Backward);
    } else {
        flippingGroup->setDirection(QAbstractAnimation::Forward);
        if (flippingGroup->currentTime() == 0) {
            //we always make sure when it is at the beginning
            rotationXanim->setStartValue(baseItem->xRotation());
            rotationYanim->setStartValue(baseItem->yRotation());
        }
    }
    flippingGroup->start();
    flipped = !flipped;
}

QPointF Panel::posForLocation(int index) const
{
    const int x = index % width,
        y = index / width;
    return QPointF(x * 150, y * 150)
        - QPointF((width - 1) * 75, (height - 1) * 75);
}
