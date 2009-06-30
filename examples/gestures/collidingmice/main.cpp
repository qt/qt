/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mouse.h"

#include "gesturerecognizerlinjazax.h"
#include "linjazaxgesture.h"

#include <QtGui>

#include <math.h>

#define ZOOMING_ANIMATION
#ifdef ZOOMING_ANIMATION
static const int AnimationSteps = 10;
#endif

static const int MouseCount = 7;

class PannableGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    PannableGraphicsView(QGraphicsScene *scene, QWidget *parent = 0)
        : QGraphicsView(scene, parent)
    {
        grabGesture(QLatin1String("LinjaZax"));
#ifdef ZOOMING_ANIMATION
        timeline = new QTimeLine(700, this);
        timeline->setFrameRange(0, AnimationSteps);
        connect(timeline, SIGNAL(frameChanged(int)), this, SLOT(animationStep(int)));
#endif
    }
protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Gesture) {
            QGestureEvent *ge = static_cast<QGestureEvent*>(event);
            const LinjaZaxGesture *g = static_cast<const LinjaZaxGesture*>(ge->gesture("LinjaZax"));
            if (g) {
                switch (g->zoomState()) {
                case LinjaZaxGesture::ZoomingIn:
#ifdef ZOOMING_ANIMATION
                    scaleStep = 1. + 0.5/AnimationSteps;
                    timeline->stop();
                    timeline->start();
#else
                    scale(1.5, 1.5);
#endif
                    break;
                case LinjaZaxGesture::ZoomingOut:
#ifdef ZOOMING_ANIMATION
                    scaleStep = 1. - 0.5/AnimationSteps;
                    timeline->stop();
                    timeline->start();
#else
                    scale(0.6, 0.6);
#endif
                    break;
                default:
                    break;
                };
                QPoint pt = g->pos() - g->lastPos();
                horizontalScrollBar()->setValue(horizontalScrollBar()->value() - pt.x());
                verticalScrollBar()->setValue(verticalScrollBar()->value() - pt.y());
                event->accept();
                return true;
            }
        }
        return QGraphicsView::event(event);
    }
private slots:
#ifdef ZOOMING_ANIMATION
    void animationStep(int step)
    {
        scale(scaleStep, scaleStep);
    }
private:
    qreal scaleStep;
    QTimeLine *timeline;
#endif
};

//! [0]
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.addGestureRecognizer(new GestureRecognizerLinjaZax);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//! [0]

//! [1]
    QGraphicsScene scene;
    scene.setSceneRect(-600, -600, 1200, 1200);
//! [1] //! [2]
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);
//! [2]

//! [3]
    for (int i = 0; i < MouseCount; ++i) {
        Mouse *mouse = new Mouse;
        mouse->setPos(::sin((i * 6.28) / MouseCount) * 200,
                      ::cos((i * 6.28) / MouseCount) * 200);
        scene.addItem(mouse);
    }
//! [3]

//! [4]
    PannableGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setBackgroundBrush(QPixmap(":/images/cheese.jpg"));
//! [4] //! [5]
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //view.setDragMode(QGraphicsView::ScrollHandDrag);
//! [5] //! [6]
    view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Colliding Mice"));
    view.resize(400, 300);
    view.show();

    return app.exec();
}
//! [6]

#include "main.moc"
