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

#include <QtCore>
#include <QtGui>
#ifdef QT_EXPERIMENTAL_SOLUTION
#include "qtgraphicswidget.h"
#endif


class Pixmap : public QGraphicsWidget
{
    Q_OBJECT

public:
    Pixmap(const QPixmap &pix, QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), orig(pix), p(pix)
    {
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->drawPixmap(QPointF(), p);
    }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent * )
    {
        emit clicked();
    }

    virtual void setGeometry(const QRectF &rect)
    {
        QGraphicsWidget::setGeometry(rect);

        if (rect.size().width() > orig.size().width())
            p = orig.scaled(rect.size().toSize());
        else
            p = orig;
    }

Q_SIGNALS:
    void clicked();

private:
    QPixmap orig;
    QPixmap p;
};

void createStateAndTransition(QObject *o1, const QRect &selectedRect, QState *parent)
{
    QState *state = new QState(parent);
    state->setPropertyOnEntry(o1, "geometry", selectedRect);

    QPropertyAnimation *animation = new QPropertyAnimation(o1, "geometry");
    parent->addAnimatedTransition(o1, SIGNAL(clicked()), state, animation);
}

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(appchooser);

    QApplication app(argc, argv);

    Pixmap *p1 = new Pixmap(QPixmap(":/digikam.png"));
    Pixmap *p2 = new Pixmap(QPixmap(":/akregator.png"));
    Pixmap *p3 = new Pixmap(QPixmap(":/accessories-dictionary.png"));
    Pixmap *p4 = new Pixmap(QPixmap(":/k3b.png"));

    p1->setObjectName("p1");
    p2->setObjectName("p2");
    p3->setObjectName("p3");
    p4->setObjectName("p4");

    p1->setGeometry(QRectF(0.0, 0.0, 64.0, 64.0));
    p2->setGeometry(QRectF(236.0, 0.0, 64.0, 64.0));
    p3->setGeometry(QRectF(236.0, 236.0, 64.0, 64.0));
    p4->setGeometry(QRectF(0.0, 236.0, 64.0, 64.0));

    QGraphicsScene scene(0, 0, 300, 300);
    scene.setBackgroundBrush(Qt::white);
    scene.addItem(p1);
    scene.addItem(p2);
    scene.addItem(p3);
    scene.addItem(p4);

    QGraphicsView window(&scene);
    window.setFrameStyle(0);
    window.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    window.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    window.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QStateMachine machine;
    machine.setGlobalRestorePolicy(QState::RestoreProperties);

    QState *group = new QState(machine.rootState());
    group->setObjectName("group");
    QRect selectedRect(86, 86, 128, 128);

    QState *idleState = new QState(group);
    group->setInitialState(idleState);

    createStateAndTransition(p1, selectedRect, group);
    createStateAndTransition(p2, selectedRect, group);
    createStateAndTransition(p3, selectedRect, group);
    createStateAndTransition(p4, selectedRect, group);

    machine.setInitialState(group);
    machine.start();

    window.resize(300, 300);
    window.show();

    return app.exec();
}

#include "main.moc"
