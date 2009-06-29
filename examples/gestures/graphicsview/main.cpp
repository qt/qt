/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

class PannableGraphicsView : public QGraphicsView
{
public:
    PannableGraphicsView()
    {
        grabGesture(Qt::PanGesture);
    }
protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Gesture) {
            QGestureEvent *gestureEvent = static_cast<QGestureEvent*>(event);
            if (const QGesture *g = gestureEvent->gesture(Qt::PanGesture)) {
                QPoint pt = g->pos() - g->lastPos();
                horizontalScrollBar()->setValue(horizontalScrollBar()->value() - pt.x());
                verticalScrollBar()->setValue(verticalScrollBar()->value() - pt.y());
                event->accept();
                return true;
            }
        }
        return QGraphicsView::event(event);
    }
};

class ImageItem : public QGraphicsItem
{
public:
    ImageItem()
        : colored(false)
    {
        grabGesture(Qt::DoubleTapGesture);
    }

    QRectF boundingRect() const
    {
        return pixmap.isNull() ? QRectF(0, 0, 100, 100)
            : QRectF(QPointF(0,0), QSizeF(pixmap.size()));
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
    {
        if (pixmap.isNull()) {
            painter->setBrush(QBrush( colored ? Qt::green : Qt::white));
            painter->drawRect(0, 0, 100, 100);
            painter->drawLine(0, 0, 100, 100);
            painter->drawLine(0, 100, 100, 0);
            return;
        }
        painter->drawPixmap(0, 0, pixmap);
    }

    bool sceneEvent(QEvent *event)
    {
        if (event->type() == QEvent::GraphicsSceneGesture) {
            QGraphicsSceneGestureEvent *gestureEvent = static_cast<QGraphicsSceneGestureEvent*>(event);
            if (gestureEvent->gesture(Qt::DoubleTapGesture)) {
                event->accept();
                colored = !colored;
                update();
                return true;
            } else {
                qWarning("Item received unknown gesture");
            }
        }
        return QGraphicsItem::sceneEvent(event);
    }

private:
    QPixmap pixmap;
    bool colored;
};

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        QVBoxLayout *l = new QVBoxLayout(this);
        view = new PannableGraphicsView;
        l->addWidget(view);
        scene = new QGraphicsScene(0, 0, 1024, 768, view);
        view->setScene(scene);

        ImageItem *item = new ImageItem;
        scene->addItem(item);
        item->setPos(scene->width()/3, scene->height()/3);
    }

signals:
public slots:
private:
    QGraphicsView *view;
    QGraphicsScene *scene;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWidget w;
    w.show();
    return app.exec();
}

#include "main.moc"
