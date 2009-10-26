/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtGui>

#include "mousepangesturerecognizer.h"

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ScrollArea(QWidget *parent = 0)
        : QScrollArea(parent), outside(false)
    {
        viewport()->grabGesture(Qt::PanGesture);
    }

protected:
    bool viewportEvent(QEvent *event)
    {
        if (event->type() == QEvent::Gesture) {
            gestureEvent(static_cast<QGestureEvent *>(event));
            return true;
        } else if (event->type() == QEvent::GestureOverride) {
            QGestureEvent *ge = static_cast<QGestureEvent *>(event);
            if (QPanGesture *pan = static_cast<QPanGesture *>(ge->gesture(Qt::PanGesture)))
                if (pan->state() == Qt::GestureStarted) {
                    outside = false;
                }
        }
        return QScrollArea::viewportEvent(event);
    }
    void gestureEvent(QGestureEvent *event)
    {
        QPanGesture *pan = static_cast<QPanGesture *>(event->gesture(Qt::PanGesture));
        if (pan) {
            switch(pan->state()) {
            case Qt::GestureStarted: qDebug("area: Pan: started"); break;
            case Qt::GestureFinished: qDebug("area: Pan: finished"); break;
            case Qt::GestureCanceled: qDebug("area: Pan: canceled"); break;
            case Qt::GestureUpdated: break;
            default: qDebug("area: Pan: <unknown state>"); break;
            }

            if (pan->state() == Qt::GestureStarted)
                outside = false;
            event->ignore();
            event->ignore(pan);
            if (outside)
                return;

            const QPointF offset = pan->offset();
            const QPointF totalOffset = pan->totalOffset();
            QScrollBar *vbar = verticalScrollBar();
            QScrollBar *hbar = horizontalScrollBar();

            if ((vbar->value() == vbar->minimum() && totalOffset.y() > 10) ||
                (vbar->value() == vbar->maximum() && totalOffset.y() < -10)) {
                outside = true;
                return;
            }
            if ((hbar->value() == hbar->minimum() && totalOffset.x() > 10) ||
                (hbar->value() == hbar->maximum() && totalOffset.x() < -10)) {
                outside = true;
                return;
            }
            vbar->setValue(vbar->value() - offset.y());
            hbar->setValue(hbar->value() - offset.x());
            event->accept(pan);
        }
    }

private:
    bool outside;
};

class Slider : public QSlider
{
public:
    Slider(Qt::Orientation orientation, QWidget *parent = 0)
        : QSlider(orientation, parent)
    {
        grabGesture(Qt::PanGesture);
    }
protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Gesture) {
            gestureEvent(static_cast<QGestureEvent *>(event));
            return true;
        }
        return QSlider::event(event);
    }
    void gestureEvent(QGestureEvent *event)
    {
        QPanGesture *pan = static_cast<QPanGesture *>(event->gesture(Qt::PanGesture));
        if (pan) {
            switch (pan->state()) {
            case Qt::GestureStarted: qDebug("slider: Pan: started"); break;
            case Qt::GestureFinished: qDebug("slider: Pan: finished"); break;
            case Qt::GestureCanceled: qDebug("slider: Pan: canceled"); break;
            case Qt::GestureUpdated: break;
            default: qDebug("slider: Pan: <unknown state>"); break;
            }

            if (pan->state() == Qt::GestureStarted)
                outside = false;
            event->ignore();
            event->ignore(pan);
            if (outside)
                return;
            const QPointF offset = pan->offset();
            const QPointF totalOffset = pan->totalOffset();
            if (orientation() == Qt::Horizontal) {
                if ((value() == minimum() && totalOffset.x() < -10) ||
                    (value() == maximum() && totalOffset.x() > 10)) {
                    outside = true;
                    return;
                }
                if (totalOffset.y() < 40 && totalOffset.y() > -40) {
                    setValue(value() + offset.x());
                    event->accept(pan);
                } else {
                    outside = true;
                }
            } else if (orientation() == Qt::Vertical) {
                if ((value() == maximum() && totalOffset.y() < -10) ||
                    (value() == minimum() && totalOffset.y() > 10)) {
                    outside = true;
                    return;
                }
                if (totalOffset.x() < 40 && totalOffset.x() > -40) {
                    setValue(value() - offset.y());
                    event->accept(pan);
                } else {
                    outside = true;
                }
            }
        }
    }
private:
    bool outside;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow()
    {
        rootScrollArea = new ScrollArea;
        setCentralWidget(rootScrollArea);

        QWidget *root = new QWidget;
        root->setFixedSize(3000, 3000);
        rootScrollArea->setWidget(root);

        Slider *verticalSlider = new Slider(Qt::Vertical, root);
        verticalSlider ->move(650, 1100);
        Slider *horizontalSlider = new Slider(Qt::Horizontal, root);
        horizontalSlider ->move(600, 1000);

        childScrollArea = new ScrollArea(root);
        childScrollArea->move(500, 500);
        QWidget *w = new QWidget;
        w->setMinimumWidth(400);
        QVBoxLayout *l = new QVBoxLayout(w);
        l->setMargin(20);
        for (int i = 0; i < 100; ++i) {
            QWidget *w = new QWidget;
            QHBoxLayout *ll = new QHBoxLayout(w);
            ll->addWidget(new QLabel(QString("Label %1").arg(i)));
            ll->addWidget(new QPushButton(QString("Button %1").arg(i)));
            l->addWidget(w);
        }
        childScrollArea->setWidget(w);
    }
private:
    ScrollArea *rootScrollArea;
    ScrollArea *childScrollArea;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.registerGestureRecognizer(new MousePanGestureRecognizer);
    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
