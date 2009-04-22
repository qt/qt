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

#include "river.h"
#include "riveritem.h"

#include "qvariantanimation.h"

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPainter>


#define GRID_ROW_COUNT 3
#define GRID_COLUMN_COUNT 2
#define GRID_DIMENSIONS (GRID_ROW_COUNT * GRID_COLUMN_COUNT)
#define ITEM_COUNT 12

#define RIVER_MAGNIFY(ITEM) (qreal(0.50) + (ITEM)->zValue()*2 )
#define GRID_MAGNIFY qreal(1.5)
#define GRID_CURRENT_MAGNIFY 2

River::River(const QString &path) :
m_images(QDir(path).entryInfoList(QStringList() << QLatin1String("*.jpg") << QLatin1String("*.png"))),
m_currentImage(0), m_mode(RiverMode), m_selectedItem(-1) , m_topLeftIndex(-1)
{
    setFocusPolicy(Qt::StrongFocus);
    setGeometry(QRectF( QPointF(), fixedSize()));


    for (int i = 0; i < ITEM_COUNT; ++i) {
        RiverItem * item = new RiverItem(this);

        //here we also randomize the x position (not when looping)
        const int x = qrand() % fixedSize().width();
        item->setPos(x, -1);

        m_items.insert(m_currentImage, item);
        addUnusedRiverItem(item);
    }

}

QPointF River::gridItemPosition(int row, int col) const
{
    if (col < 0) {
        col += GRID_COLUMN_COUNT;
        row --;
    }
    return  QPointF(rect().width()*(col*2 + 1)/(GRID_COLUMN_COUNT*2),
        rect().height()*(row*2 + 1)/(GRID_ROW_COUNT*2));
}

QPixmap River::pixmap(int index) const
{
    if (index < 0 || index >= m_images.size())
        return QPixmap();

    if (m_pixmaps.size() <= index) {
        m_pixmaps.resize(index+1);
    }

    if (m_pixmaps.at(index).isNull()) {
        m_pixmaps[index] = QPixmap(m_images.at(index).absoluteFilePath());
    }

    return m_pixmaps.at(index);
}

void River::addUnusedRiverItem(RiverItem * item)
{
    if (m_images.isEmpty())
        return;

    QRectF realItemRect = item->mapToParent(item->boundingRect()).boundingRect();

    int y = item->pos().y();
    int x = item->pos().x();
    if (x >= fixedSize().width() || x < -realItemRect.width() || y < 0) {
        //we set the new pixmap

        m_items.remove(m_items.key(item));

        while (m_items.contains(m_currentImage))
            m_currentImage = (m_currentImage + 1 ) % m_images.size();

        item->setPixmap(pixmap(m_currentImage));

        m_items.insert(m_currentImage, item);
        //this manages looping as well
        m_currentImage = (m_currentImage + 1 ) % m_images.size();

        item->setZValue(qreal(qrand()%100)/200.0);

        QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::ScaleFactorX, scene());
        anim->setEndValue(RIVER_MAGNIFY(item));
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        anim = new QItemAnimation(item, QItemAnimation::ScaleFactorY, scene());
        anim->setEndValue(RIVER_MAGNIFY(item));
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        realItemRect = item->mapToParent(item->boundingRect()).boundingRect();

        y = -realItemRect.y() + qrand() % (fixedSize().height() - int(realItemRect.height()));
        if (x >= fixedSize().width()) {
            x = -realItemRect.width()/2;
        }
    }

    item->setPos(x, y);

    const QPointF target(QPointF(fixedSize().width() + realItemRect.width()/2, y));

    const int distance = target.x() - x;

    const int duration = (40 - 50*item->zValue() ) * distance;
    QItemAnimation *a =  new QItemAnimation(item, QItemAnimation::Position, scene());
    a->setEndValue(target);
    a->setDuration(duration);
    a->start(QAbstractAnimation::DeleteWhenStopped);
    connect(a, SIGNAL(finished()), SLOT(animationFinished()));
}

void River::animationFinished()
{
    QItemAnimation *anim = qobject_cast<QItemAnimation*>(sender());
    if (!anim || anim->propertyName() != QItemAnimation::Position)
        return;

    /*RiverItem *item = static_cast<RiverItem*>(anim->graphicsItem());
    if (m_mode != RiverMode) {*/
        /*int key = m_items.key(item);
        if (key < m_topLeftIndex || key >= m_topLeftIndex + GRID_DIMENSIONS) {
            delete item;
            m_items.remove(key);
        }*/
        //return;

    //}

    addUnusedRiverItem(static_cast<RiverItem*>(anim->targetItem()));
}

void River::switchPaused()
{
    const bool paused = m_pausedAnimations.isEmpty();
    if (paused)
        m_pausedAnimations = scene()->findChildren<QItemAnimation*>();

    foreach(QItemAnimation *anim, m_pausedAnimations) {
        if (paused)
            anim->pause();
        else
            anim->resume();
    }

    if (!paused)
        m_pausedAnimations.clear();
}

void River::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->fillRect(option->rect, Qt::black);
}

void River::setMode(Mode m)
{
    if (m_mode == m)
        return;

    Mode oldMode = m_mode;
    m_mode = m;
    switch(m)
    {
    case RiverMode:
        m_mode = oldMode; //some animation may be stopt, and we don't want that animationFinished we were in that mode yet
        foreach (RiverItem *item, m_items) {
            const int x = qrand() % fixedSize().width();
            const int y = qrand() % fixedSize().width();
            QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::ScaleFactorX, scene());
            anim->setEndValue(RIVER_MAGNIFY(item));
            anim->start(QAbstractAnimation::DeleteWhenStopped);
            anim = new QItemAnimation(item, QItemAnimation::ScaleFactorY, scene());
            anim->setEndValue(RIVER_MAGNIFY(item));
            anim->start(QAbstractAnimation::DeleteWhenStopped);
            anim = new QItemAnimation(item, QItemAnimation::Position, scene());
            anim->setEndValue(QPointF(x, y));
            anim->start(QAbstractAnimation::DeleteWhenStopped);
            connect(anim, SIGNAL(finished()), SLOT(animationFinished()));
        }
        m_mode = m;
        break;

    case GridMode:

        if (oldMode == GridFullScreenMode) {
            currentItem()->setFullScreen(false, GRID_CURRENT_MAGNIFY);
        } else {
            m_topLeftIndex = -GRID_DIMENSIONS;
            foreach (RiverItem *item, m_items) {
                QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::ScaleFactorX, scene());
                anim->setEndValue(GRID_MAGNIFY);
                anim->start(QAbstractAnimation::DeleteWhenStopped);
                anim = new QItemAnimation(item, QItemAnimation::ScaleFactorY, scene());
                anim->setEndValue(GRID_MAGNIFY);
                anim->start(QAbstractAnimation::DeleteWhenStopped);
            }
            adjustGrid(m_currentImage - GRID_DIMENSIONS + 1);
            setCurrentItem(m_topLeftIndex);
        }
        break;
    case GridFullScreenMode:
        //let's put the current item fullscreen
        currentItem()->setFullScreen(true, GRID_CURRENT_MAGNIFY);
        break;
    case CoverMode:
        m_gridItem = m_items.values();
        setCurrentItem(m_gridItem.count()/2);
    default:
        break;
    }
}

River::Mode River::mode() const
{
    return m_mode;
}

QSize River::fixedSize()
{
    return QSize(352, 416);
}

//the name of this method is not that good...
void River::makeCenterAvailable(qreal size)
{
    QRectF center(QPointF(), QSizeF(size, size));
    center.moveCenter(rect().center());

    const QList<QGraphicsItem*> list = scene()->items(center);

    foreach(QGraphicsItem *item, m_items) {

        if (!list.contains(item)) 
            continue;

        QPointF pos = item->pos();

        if (pos.y() < rect().center().y()) {
            //item is above center
            pos.ry() = center.top() - item->boundingRect().height() - 1;
        } else {
            //item is below the center
            pos.ry() = center.bottom() + 1;
        }
        QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::Position, scene());
        anim->setEndValue(pos);
        anim->setDuration(150);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}


//this is there just to test small interaction
void River::keyPressEvent ( QKeyEvent * keyEvent )
{
    switch(keyEvent->key())
    {
    case Qt::Key_O:
        {
            QItemAnimation *anim = new QItemAnimation(this, QItemAnimation::Opacity, scene());
            anim->setDuration(2000);
            anim->setEndValue(qreal(.5));
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        }
        break;
    case Qt::Key_N:
        //that's a test
        makeCenterAvailable(60);
        break;
    case Qt::Key_P:
        switchPaused();
        break;
    case Qt::Key_V:
        setMode(GridMode);
        break;
    case Qt::Key_R:
        setMode(RiverMode);
        break;
    case Qt::Key_C:
        setMode(CoverMode);
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_mode == RiverMode) {
            setMode(GridMode);
        } else if (m_mode == GridMode) {
            setMode(GridFullScreenMode);
        } else if (m_mode == GridFullScreenMode) {
            setMode(GridMode);
        }
        break;
    case Qt::Key_Escape:
        if (m_mode == GridFullScreenMode) {
            setMode(GridMode);
        } else if (m_mode == GridMode  || m_mode == CoverMode) {
            setMode(RiverMode);
        } else if (m_mode == RiverMode) {
            menu->show();
        }
        break;
    case Qt::Key_Right:
        if (m_mode == GridMode) {
            navigateBy(+1);
        } else if (m_mode == CoverMode) {
            setCurrentItem(m_selectedItem + 1);
        }
        break;
    case Qt::Key_Left:
        if (m_mode == GridMode) {
            navigateBy(-1);
        } else if (m_mode == CoverMode) {
            setCurrentItem(m_selectedItem - 1);
        }
        break;
    case Qt::Key_Down:
        if (m_mode == GridMode) {
            navigateBy(GRID_COLUMN_COUNT);
        }
        break;
    case Qt::Key_Up:
        if (m_mode == GridMode) {
            navigateBy(-GRID_COLUMN_COUNT);
        }
        break;
//    case Qt::Key_PageUp:
    case Qt::Key_M:
        menu->show();
        break;
    case Qt::Key_Space:
        if (m_mode == GridMode) {
            RiverItem *item = currentItem();
            if(!item)
                break;

            //stupid sequence.
            QPointF pos = item->pos();
            QAnimationGroup *group = new QSequentialAnimationGroup();
            //item->animator()->beginSequence();

            QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::Position, scene());
            anim->setEndValue(pos);
            group->addAnimation(anim);
            anim = new QItemAnimation(item, QItemAnimation::ScaleFactorX, scene());
            anim->setEndValue(qreal(1.));
            anim->setDuration(500);
            group->addAnimation(anim);
            anim = new QItemAnimation(item, QItemAnimation::ScaleFactorY, scene());
            anim->setEndValue(qreal(1.));
            anim->setDuration(500);
            group->addAnimation(anim);
            anim = new QItemAnimation(item, QItemAnimation::RotationX, scene());
            anim->setEndValue(qreal(0));
            group->addAnimation(anim);
            group->start(QAbstractAnimation::DeleteWhenStopped);
        }
    default:
        break;
    }

    QGraphicsItem::keyPressEvent(keyEvent);
}


void River::setGridMode()
{
    setMode(GridMode);
}

void River::setRiverMode()
{
    setMode(RiverMode);
}

void River::setCoverMode()
{
    setMode(CoverMode);
}
 
 
void River::adjustGrid(int newTopLeft)
{
    for (int i = newTopLeft ; i < newTopLeft + GRID_DIMENSIONS; i++) {
        if (!m_items.contains(i)) {
            RiverItem *item = createItem(i);
            int row = (i - m_topLeftIndex) / GRID_COLUMN_COUNT;
            int col = (i - m_topLeftIndex) % GRID_COLUMN_COUNT;
            item->setPos(gridItemPosition(row, col));
            item->setXScale(0);
            item->setYScale(0);
            QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::ScaleFactorX, scene());
            anim->setEndValue(GRID_MAGNIFY);
            anim->start(QAbstractAnimation::DeleteWhenStopped);
            anim = new QItemAnimation(item, QItemAnimation::ScaleFactorY, scene());
            anim->setEndValue(GRID_MAGNIFY);
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
    newTopLeft = newTopLeft - newTopLeft % GRID_COLUMN_COUNT;

    QHash<int, RiverItem *>::iterator it = m_items.begin();
    while (it != m_items.constEnd()) {
        const int imageIdx = it.key();
        RiverItem *item = *it;
        QSizeF itemSize = item->boundingRect().size();
        if ((imageIdx >= newTopLeft && imageIdx < newTopLeft + GRID_DIMENSIONS)
            || boundingRect().adjusted(-itemSize.width()/2, -itemSize.height()/2, itemSize.width()/2, itemSize.height()/2)
                        .contains(item->pos())) {
            int row = (imageIdx-newTopLeft) / GRID_COLUMN_COUNT;
            int col = (imageIdx-newTopLeft) % GRID_COLUMN_COUNT;
            QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::Position, scene());
            anim->setEndValue(gridItemPosition(row, col));
            anim->start(QAbstractAnimation::DeleteWhenStopped);
            ++it;
        } else {
            ++it; /* ### ideally we should remove the items, but this cause the photobrowser to crash
                         because the QItemAnimations are not notified the item is deleted
            delete item;
            it = m_items.erase(it);
            */
        }
    }
    
    m_topLeftIndex = newTopLeft;
}


RiverItem *River::createItem(int imageIndex) 
{
    Q_ASSERT(!m_items.contains(imageIndex));
    RiverItem * item = new RiverItem(this);
    item->setPixmap(pixmap(imageIndex));
    m_items.insert(imageIndex,item);
    return item;
}

void River::setCurrentItem(int newCurrentItem)
{
    if (m_mode == CoverMode)
    {
        m_selectedItem = newCurrentItem;
        for (int i = 0; i < m_gridItem.count(); i++) {
            QVariantAnimation *anim;
            RiverItem *item = m_gridItem.at(i);

            qreal rotation = 0;
            qreal width = boundingRect().width() / 2;
            qreal x = width;
            qreal scale = 3;
            qreal z = 1;
            qreal y = boundingRect().height()/2;

            if (i < newCurrentItem - 4) {
                item->setVisible(false);
                item->setPos(QPointF(0, y));
                continue;
            } else if(i > newCurrentItem + 4) {
                item->setVisible(false);
                item->setPos(QPointF(boundingRect().width(), y));
                continue;
            } else if (i < newCurrentItem) { 
                x = (i - newCurrentItem + 4) * width/7;
                rotation = -75;
                scale = 2;
                z = 1.+qreal(i-newCurrentItem)/10.;
            } else if (i > newCurrentItem) { 
                x =  width + (i - newCurrentItem + 3) * width/7;
                rotation = 75;
                scale = 2;
                z = 1.-qreal(i-newCurrentItem)/8.;
            }

            item->setVisible(true);
            item->setZValue(z);

            anim = new QItemAnimation(item, QItemAnimation::RotationY, scene());
            anim->setEndValue(rotation);
            anim->start(QAbstractAnimation::DeleteWhenStopped);

            anim = new QItemAnimation(item, QItemAnimation::ScaleFactorX, scene());
            anim->setEndValue(scale);
            anim->start(QVariantAnimation::DeleteWhenStopped);

            anim = new QItemAnimation(item, QItemAnimation::ScaleFactorY, scene());
            anim->setEndValue(scale);
            anim->start(QVariantAnimation::DeleteWhenStopped);

            anim = new QItemAnimation(item, QItemAnimation::Position, scene());
            anim->setEndValue(QPointF(x,y));
            anim->start(QVariantAnimation::DeleteWhenStopped);
        }
        return;
    }


    //deselect the current item
    if (m_selectedItem >= 0 && m_items.contains(m_selectedItem)) {
        RiverItem *item = m_items.value(m_selectedItem);
        item->setZValue(qreal(qrand()%100)/200.0);
        QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::ScaleFactorX, scene());
        anim->setEndValue(GRID_MAGNIFY);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        anim = new QItemAnimation(item, QItemAnimation::ScaleFactorY, scene());
        anim->setEndValue(GRID_MAGNIFY);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    if (newCurrentItem < 0) {
        m_selectedItem = newCurrentItem;
        return;
    }

    //ensure visible;
    if (newCurrentItem < m_topLeftIndex) {
        adjustGrid(newCurrentItem);
    } else if (newCurrentItem >= m_topLeftIndex + GRID_DIMENSIONS) {
        adjustGrid(newCurrentItem - GRID_DIMENSIONS  + GRID_COLUMN_COUNT);
    }

    //select the new one
    m_selectedItem = newCurrentItem;
    RiverItem *item = currentItem();
    Q_ASSERT(item);
    item->setZValue(1);

    QItemAnimation *anim = new QItemAnimation(item, QItemAnimation::ScaleFactorX, scene());
    anim->setEndValue(GRID_CURRENT_MAGNIFY);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    anim = new QItemAnimation(item, QItemAnimation::ScaleFactorY, scene());
    anim->setEndValue(GRID_CURRENT_MAGNIFY);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void River::navigateBy(int offset)
{
    int newSelection = m_selectedItem + offset;
    const int imageCount = m_images.size();
    while (newSelection < 0)
        newSelection += imageCount;
    newSelection %= imageCount;
    setCurrentItem(newSelection);
}
