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
#if defined(QT_EXPERIMENTAL_SOLUTION)
#include "qstatemachine.h"
#include "qstate.h"
#include "qabstracttransition.h"
#include "qpropertyanimation.h"
#include "qsequentialanimationgroup.h"
#include "qparallelanimationgroup.h"
#include "qgraphicswidget.h"
#endif
#include <time.h>

class StateSwitchEvent: public QEvent
{
public:
    StateSwitchEvent()
        : QEvent(Type(StateSwitchType))
    {
    }

    StateSwitchEvent(int rand)
        : QEvent(Type(StateSwitchType)),
          m_rand(rand)
    {
    }

    enum { StateSwitchType = QEvent::User + 256 };

    int rand() const { return m_rand; }

private:
    int m_rand;
};


class QGraphicsRectWidget : public QGraphicsWidget
{
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
               QWidget *)
    {
        painter->fillRect(rect(), Qt::blue);
    }
};

class StateSwitchTransition: public QAbstractTransition
{
public:
    StateSwitchTransition(int rand)
        : QAbstractTransition(),
          m_rand(rand)
    {
    }

protected:
    virtual bool eventTest(QEvent *event) const
    {
        return (event->type() == QEvent::Type(StateSwitchEvent::StateSwitchType))
            && (static_cast<StateSwitchEvent *>(event)->rand() == m_rand);
    }

    virtual void onTransition() {}

private:
    int m_rand;
};

class StateSwitcher : public QState
{
    Q_OBJECT
public:
    StateSwitcher(QStateMachine *machine)
        : QState(machine->rootState()), m_machine(machine),
          m_stateCount(0), m_lastIndex(0)
    { }

    virtual void onEntry()
    {
        int n;
        while ((n = (qrand() % m_stateCount + 1)) == m_lastIndex)
        { }
        m_lastIndex = n;
        m_machine->postEvent(new StateSwitchEvent(n));
    }
    virtual void onExit() {}

    void addState(QState *state, QAbstractAnimation *animation) {
        StateSwitchTransition *trans = new StateSwitchTransition(++m_stateCount);
        trans->setTargetState(state);
        addTransition(trans);
        trans->addAnimation(animation);
    }


private:
    QStateMachine *m_machine;
    int m_stateCount;
    int m_lastIndex;
};

QState *createGeometryState(QObject *w1, const QRect &rect1,
                            QObject *w2, const QRect &rect2,
                            QObject *w3, const QRect &rect3,
                            QObject *w4, const QRect &rect4,
                            QState *parent)
{
    QState *result = new QState(parent);
    result->assignProperty(w1, "geometry", rect1);
    result->assignProperty(w1, "geometry", rect1);
    result->assignProperty(w2, "geometry", rect2);
    result->assignProperty(w3, "geometry", rect3);
    result->assignProperty(w4, "geometry", rect4);

    return result;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

#if 0
    QWidget window;
    QPalette palette;
    palette.setBrush(QPalette::Window, Qt::black);
    window.setPalette(palette);
    QPushButton *button1 = new QPushButton("A", &window);
    QPushButton *button2 = new QPushButton("B", &window);
    QPushButton *button3 = new QPushButton("C", &window);
    QPushButton *button4 = new QPushButton("D", &window);

    button1->setObjectName("button1");
    button2->setObjectName("button2");
    button3->setObjectName("button3");
    button4->setObjectName("button4");
#else
    QGraphicsRectWidget *button1 = new QGraphicsRectWidget;
    QGraphicsRectWidget *button2 = new QGraphicsRectWidget;
    QGraphicsRectWidget *button3 = new QGraphicsRectWidget;
    QGraphicsRectWidget *button4 = new QGraphicsRectWidget;
    button2->setZValue(1);
    button3->setZValue(2);
    button4->setZValue(3);
    QGraphicsScene scene(0, 0, 300, 300);
    scene.setBackgroundBrush(Qt::black);
    scene.addItem(button1);
    scene.addItem(button2);
    scene.addItem(button3);
    scene.addItem(button4);

    QGraphicsView window(&scene);
    window.setFrameStyle(0);
    window.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    window.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    window.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
    QStateMachine machine;

    QState *group = new QState();
    group->setObjectName("group");
    QTimer timer;
    timer.setInterval(1250);
    timer.setSingleShot(true);
    QObject::connect(group, SIGNAL(entered()), &timer, SLOT(start()));

    QState *state1;
    QState *state2;
    QState *state3;
    QState *state4;
    QState *state5;
    QState *state6;
    QState *state7;

    state1 = createGeometryState(button1, QRect(100, 0, 50, 50),
                                 button2, QRect(150, 0, 50, 50),
                                 button3, QRect(200, 0, 50, 50),
                                 button4, QRect(250, 0, 50, 50),
                                 group);
    state2 = createGeometryState(button1, QRect(250, 100, 50, 50),
                                 button2, QRect(250, 150, 50, 50),
                                 button3, QRect(250, 200, 50, 50),
                                 button4, QRect(250, 250, 50, 50),
                                 group);
    state3 = createGeometryState(button1, QRect(150, 250, 50, 50),
                                 button2, QRect(100, 250, 50, 50),
                                 button3, QRect(50, 250, 50, 50),
                                 button4, QRect(0, 250, 50, 50),
                                 group);
    state4 = createGeometryState(button1, QRect(0, 150, 50, 50),
                                 button2, QRect(0, 100, 50, 50),
                                 button3, QRect(0, 50, 50, 50),
                                 button4, QRect(0, 0, 50, 50),
                                 group);
    state5 = createGeometryState(button1, QRect(100, 100, 50, 50),
                                 button2, QRect(150, 100, 50, 50),
                                 button3, QRect(100, 150, 50, 50),
                                 button4, QRect(150, 150, 50, 50),
                                 group);
    state6 = createGeometryState(button1, QRect(50, 50, 50, 50),
                                 button2, QRect(200, 50, 50, 50),
                                 button3, QRect(50, 200, 50, 50),
                                 button4, QRect(200, 200, 50, 50),
                                 group);
    state7 = createGeometryState(button1, QRect(0, 0, 50, 50),
                                 button2, QRect(250, 0, 50, 50),
                                 button3, QRect(0, 250, 50, 50),
                                 button4, QRect(250, 250, 50, 50),
                                 group);
    group->setInitialState(state1);

    QParallelAnimationGroup animationGroup;
    QSequentialAnimationGroup *subGroup;

    QPropertyAnimation *anim = new QPropertyAnimation(button4, "geometry");
    anim->setDuration(1000);
    anim->setEasingCurve(QEasingCurve::OutElastic);
    animationGroup.addAnimation(anim);

    subGroup = new QSequentialAnimationGroup(&animationGroup);
    subGroup->addPause(100);
    anim = new QPropertyAnimation(button3, "geometry");
    anim->setDuration(1000);
    anim->setEasingCurve(QEasingCurve::OutElastic);
    subGroup->addAnimation(anim);

    subGroup = new QSequentialAnimationGroup(&animationGroup);
    subGroup->addPause(150);
    anim = new QPropertyAnimation(button2, "geometry");
    anim->setDuration(1000);
    anim->setEasingCurve(QEasingCurve::OutElastic);
    subGroup->addAnimation(anim);

    subGroup = new QSequentialAnimationGroup(&animationGroup);
    subGroup->addPause(200);
    anim = new QPropertyAnimation(button1, "geometry");
    anim->setDuration(1000);
    anim->setEasingCurve(QEasingCurve::OutElastic);
    subGroup->addAnimation(anim);

    StateSwitcher *stateSwitcher = new StateSwitcher(&machine);
    stateSwitcher->setObjectName("stateSwitcher");
    group->addTransition(&timer, SIGNAL(timeout()), stateSwitcher);
    stateSwitcher->addState(state1, &animationGroup);
    stateSwitcher->addState(state2, &animationGroup);
    stateSwitcher->addState(state3, &animationGroup);
    stateSwitcher->addState(state4, &animationGroup);
    stateSwitcher->addState(state5, &animationGroup);
    stateSwitcher->addState(state6, &animationGroup);
    stateSwitcher->addState(state7, &animationGroup);

    machine.addState(group);
    machine.setInitialState(group);
    machine.start();


    window.resize(300, 300);
    window.show();

    qsrand(time(0));

    return app.exec();
}

#include "main.moc"
