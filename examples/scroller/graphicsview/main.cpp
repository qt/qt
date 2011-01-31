/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore>
#include <QtGui>

#define NUM_ITEMS 100
#define NUM_LISTS 10

/*!
    \class RectObject
    Note that it needs to be a QGraphicsObject or else the gestures will not work correctly.
*/
class RectObject : public QGraphicsObject
{
    Q_OBJECT

public:

    RectObject(const QString &text, qreal x, qreal y, qreal width, qreal height, QBrush brush, QGraphicsItem *parent = 0)
        : QGraphicsObject(parent)
        , m_text(text)
        , m_rect(x, y, width, height)
        , m_pen(brush.color().lighter(), 3.0)
        , m_brush(brush)
    {
        setFlag(QGraphicsItem::ItemClipsToShape, true);
    }

    QRectF boundingRect() const
    {
        // here we only want the size of the children and not the size of the children of the children...
        qreal halfpw = m_pen.widthF() / 2;
        QRectF rect = m_rect;
        if (halfpw > 0.0)
            rect.adjust(-halfpw, -halfpw, halfpw, halfpw);

        return rect;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setPen(m_pen);
        painter->setBrush(m_brush);
        painter->drawRect(m_rect);

        painter->setPen(Qt::black);
        QFont f;
        f.setPixelSize(m_rect.height());
        painter->setFont(f);
        painter->drawText(m_rect, Qt::AlignCenter, m_text);
    }

    QString m_text;
    QRectF m_rect;
    QPen m_pen;
    QBrush m_brush;
};

class ViewObject : public QGraphicsObject
{
    Q_OBJECT
public:
    ViewObject(QGraphicsObject *parent)
        : QGraphicsObject(parent)
    { }

    QRectF boundingRect() const
    {
        QRectF rect;
        foreach (QGraphicsItem *item, childItems())
            rect |= item->boundingRect().translated(item->pos());
        return rect;
    }

    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
    { }
};

class ListObject : public QGraphicsObject
{
    Q_OBJECT

public:
    ListObject(const QSizeF &size, bool useTouch)
    {
        m_size = size;
        setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
        // grab gesture via Touch or Mouse events
        QScroller::grabGesture(this, useTouch ? QScroller::TouchGesture : QScroller::LeftMouseButtonGesture);

        // this needs to be QGraphicsOBJECT - otherwise gesture recognition
        // will not work for the parent of the viewport (in this case the
        // list)
        m_viewport = new ViewObject(this);

    }

    QGraphicsObject *viewport() const
    {
        return m_viewport;
    }

    bool event(QEvent *e)
    {
        switch (e->type()) {
// ![2]
        case QEvent::ScrollPrepare: {
            QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(e);
            se->setViewportSize(m_size);
            QRectF br = m_viewport->boundingRect();
            se->setContentPosRange(QRectF(0, 0,
                                          qMax(qreal(0), br.width() - m_size.width()),
                                          qMax(qreal(0), br.height() - m_size.height())));
            se->setContentPos(-m_viewport->pos());
            se->accept();
            return true;
        }
// ![1]
// ![2]
        case QEvent::Scroll: {
            QScrollEvent *se = static_cast<QScrollEvent *>(e);
            m_viewport->setPos(-se->contentPos() - se->overshootDistance());
            return true;
        }
// ![2]
        default:
            break;
        }
        return QGraphicsObject::event(e);
    }

    bool sceneEvent(QEvent *e)
    {
        switch (e->type()) {
        case QEvent::TouchBegin: {
            // We need to return true for the TouchBegin here in the
            // top-most graphics object - otherwise gestures in our parent
            // objects will NOT work at all (the accept() flag is already
            // set due to our setAcceptTouchEvents(true) call in the c'tor
            return true;

        }
        case QEvent::GraphicsSceneMousePress: {
            // We need to return true for the MousePress here in the
            // top-most graphics object - otherwise gestures in our parent
            // objects will NOT work at all (the accept() flag is already
            // set to true by Qt)
            return true;

        }
        default:
            break;
        }
        return QGraphicsObject::sceneEvent(e);
    }

    QRectF boundingRect() const
    {
        return QRectF(0, 0, m_size.width() + 3, m_size.height());
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setPen(QPen(QColor(100, 100, 100), 3.0));
        painter->drawRect(QRect(1.5, 1.5, m_size.width() - 3, m_size.height() - 3));
    }

    QSizeF m_size;
    ViewObject *m_viewport;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(bool useTouch)
    {
        m_scene = new QGraphicsScene();

        // -- make the main list
        ListObject *mainList = new ListObject(QSizeF(780, 400), useTouch);
        mainList->setObjectName(QLatin1String("MainList"));
        m_scene->addItem(mainList);
// ![3]
        for (int i=0; i<NUM_LISTS; i++) {
            ListObject *childList = new ListObject(QSizeF(mainList->m_size.width()/3, mainList->m_size.height()), useTouch);
            childList->setObjectName(QString("ChildList %1").arg(i));
            fillList(childList);
            childList->setParentItem(mainList->viewport());
            childList->setPos(i*mainList->m_size.width()/3, 0);
        }
        mainList->viewport()->setPos(0, 0);


        /*
        list1->setTransformOriginPoint(200, 200);
        list1->setRotation(135);
        list1->setPos(20 + 200 * .41, 20 + 200 * .41);
        */
// ![3]

        m_view = new QGraphicsView(m_scene);
        setCentralWidget(m_view);
        setWindowTitle(tr("Gesture example"));
        m_scene->setSceneRect(0, 0, m_view->viewport()->width(), m_view->viewport()->height());
    }

    /**
     *  Fills the list object \a list with RectObjects.
     */
    void fillList(ListObject *list)
    {
        qreal h = list->m_size.height() / 10;
        for (int i=0; i<NUM_ITEMS; i++) {
            QColor color =  QColor(255*i/NUM_ITEMS, 255*(NUM_ITEMS-i)/NUM_ITEMS, 127*(i%2)+64*(i/2%2));
            QString text = QLatin1String("Item #") + QString::number(i);
            QGraphicsItem *rect = new RectObject(text, 0, 0, list->m_size.width() - 6, h - 3, QBrush(color), list->viewport());
            rect->setPos(3, h*i+3);
        }
        list->viewport()->setPos(0, 0);
    }


protected:
    void resizeEvent(QResizeEvent *e)
    {
        // resize the scene according to our own size to prevent scrolling
        m_scene->setSceneRect(0, 0, m_view->viewport()->width(), m_view->viewport()->height());
        QMainWindow::resizeEvent(e);
    }

    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bool touch = (a.arguments().contains(QLatin1String("--touch")));
    MainWindow mw(touch);
#ifdef Q_WS_S60
    mw.showMaximized();
#else
    mw.show();
#endif
#ifdef Q_WS_MAC
    mw.raise();
#endif
    return a.exec();
}

#include "main.moc"
