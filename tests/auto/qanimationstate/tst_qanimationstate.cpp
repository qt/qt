/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtCore/qstate.h>
#include <QtCore/qstatemachine.h>
#include <QtCore/qanimationstate.h>

//TESTED_CLASS=QAnimationState
//TESTED_FILES=

#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)


class tst_QAnimationState : public QObject
{
    Q_OBJECT
public:
    tst_QAnimationState();
    virtual ~tst_QAnimationState();

private slots:
    void init();
    void cleanup();
    void construction();
    void noAnimation();
    void simpleAnimation();
    void twoAnimations();
    void reuseAnimation();
    void nestedTargetState();
    void parallelTargetState();
    void playTwice();
    void twoAnimatedTransitions();
    void globalRestoreProperty();
    void specificRestoreProperty();
    void someAnimationsNotSpecified();
    void someActionsNotAnimated();
    void specificTargetValueOfAnimation();
    void persistentTargetValueOfAnimation();
};

tst_QAnimationState::tst_QAnimationState()
{
}

tst_QAnimationState::~tst_QAnimationState()
{
}

void tst_QAnimationState::init()
{
}

void tst_QAnimationState::cleanup()
{
}

void tst_QAnimationState::construction()
{
    QAnimationState as;
}

class EventTransition : public QTransition
{
public:
    EventTransition(QEvent::Type type, QAbstractState *target)
        : QTransition(), m_type(type) {
            setTargetState(target);
        }
protected:
    virtual bool eventTest(QEvent *e) const {
        return (e->type() == m_type);
    }
private:
    QEvent::Type m_type;
};

void tst_QAnimationState::noAnimation()
{
    QStateMachine machine;

    QState *s1 = new QState(machine.rootState());
    QState *s2 = new QState(machine.rootState());
    s2->setProperty("entered", false);
    s2->setPropertyOnEntry(s2, "entered", true);

    s1->addAnimatedTransition(new EventTransition(QEvent::User, s2));
    s2->addTransition(new EventTransition(QEvent::User, s1));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    QVERIFY(machine.configuration().contains(s1));

    machine.postEvent(new QEvent(QEvent::User));

    QTRY_COMPARE(s2->property("entered").toBool(), true);
    QVERIFY(machine.configuration().contains(s2));

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QVERIFY(machine.configuration().contains(s1));

    s2->setProperty("entered", false);
    machine.postEvent(new QEvent(QEvent::User));

    QTRY_COMPARE(s2->property("entered").toBool(), true);
    QVERIFY(machine.configuration().contains(s2));
}

class ValueCheckerState: public QState
{
public:
    ValueCheckerState(QState *parent)
        : QState(parent)
    {
    }

    void addPropertyToCheck(const QObject *object, const char *propertyName)
    {
        m_objects.append(object);
        m_propertyNames.append(propertyName);
        valueOnEntry.append(QVariant());
    }

    QVariantList valueOnEntry;

protected:
    virtual void onEntry()
    {
        for (int i=0; i<m_objects.size(); ++i)
            valueOnEntry[i] = m_objects.at(i)->property(m_propertyNames.at(i));

        QState::onEntry();
    }

    QList<const QObject *> m_objects;
    QList<QByteArray> m_propertyNames;

};

void tst_QAnimationState::simpleAnimation()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("fooBar", 1.0);

    QState *s1 = new QState(machine.rootState());
    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "fooBar");
    s2->setPropertyOnEntry(object, "fooBar", 2.0);

    QPropertyAnimation *animation = new QPropertyAnimation(object, "fooBar", s2);
    s1->addAnimatedTransition(new EventTransition(QEvent::User, s2), animation);

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));

    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);

    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 2.0);
}

void tst_QAnimationState::twoAnimations()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);
    object->setProperty("bar", 3.0);

    QState *s1 = new QState(machine.rootState());
    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->addPropertyToCheck(object, "bar");
    s2->setPropertyOnEntry(object, "foo", 2.0);
    s2->setPropertyOnEntry(object, "bar", 10.0);

    QPropertyAnimation *animationFoo = new QPropertyAnimation(object, "foo", s2);
    QPropertyAnimation *animationBar = new QPropertyAnimation(object, "bar", s2);
    animationBar->setDuration(900);
    QAnimationState *as = s1->addAnimatedTransition(new EventTransition(QEvent::User, s2));
    as->addAnimation(animationFoo);
    as->addAnimation(animationBar);

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));

    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);

    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 2.0);
    QCOMPARE(s2->valueOnEntry.at(1).toDouble(), 10.0);
}

void tst_QAnimationState::parallelTargetState()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);
    object->setProperty("bar", 3.0);

    QState *s1 = new QState(machine.rootState());
    QState *s2 = new QState(QState::ParallelGroup, machine.rootState());

    ValueCheckerState *c1 = new ValueCheckerState(s2);
    c1->setPropertyOnEntry(object, "foo", 2.0);
    c1->addPropertyToCheck(object, "foo");
    c1->addPropertyToCheck(object, "bar");

    QState *c2 = new QState(s2);
    c2->setPropertyOnEntry(object, "bar", 10.0);

    QAnimationState *as = s1->addAnimatedTransition(new EventTransition(QEvent::User, s2));
    as->addAnimation(new QPropertyAnimation(object, "foo", as));
    as->addAnimation(new QPropertyAnimation(object, "bar", as));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));

    QTRY_COMPARE(c1->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(c1->valueOnEntry.at(0).toDouble(), 2.0);
    QCOMPARE(c1->valueOnEntry.at(1).toDouble(), 10.0);
}


void tst_QAnimationState::twoAnimatedTransitions()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);

    QState *s1 = new QState(machine.rootState());
    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->setPropertyOnEntry(object, "foo", 5.0);
    ValueCheckerState *s3 = new ValueCheckerState(machine.rootState());
    s3->setPropertyOnEntry(object, "foo", 2.0);
    s3->addPropertyToCheck(object, "foo");

    s1->addAnimatedTransition(new EventTransition(QEvent::User, s2),
                              new QPropertyAnimation(object, "foo", s2));
    s2->addAnimatedTransition(new EventTransition(QEvent::User, s3),
                              new QPropertyAnimation(object, "foo", s2));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 5.0);

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s3->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s3->valueOnEntry.at(0).toDouble(), 2.0);
}

void tst_QAnimationState::playTwice()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);

    QState *s1 = new QState(machine.rootState());
    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->setPropertyOnEntry(object, "foo", 5.0);
    QState *s3 = new QState(machine.rootState());
    s3->setPropertyOnEntry(object, "foo", 2.0);

    s1->addAnimatedTransition(new EventTransition(QEvent::User, s2),
                              new QPropertyAnimation(object, "foo", s2));
    s2->addTransition(new EventTransition(QEvent::User, s3));
    s3->addTransition(s1);

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 5.0);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();
    QVERIFY(machine.configuration().contains(s1));
    QCOMPARE(object->property("foo").toDouble(), 2.0);

    s2->valueOnEntry[0] = QVariant();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(object->property("foo").toDouble(), 5.0);
}

void tst_QAnimationState::nestedTargetState()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);
    object->setProperty("bar", 3.0);

    QState *s1 = new QState(machine.rootState());
    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->addPropertyToCheck(object, "bar");
    s2->setPropertyOnEntry(object, "foo", 2.0);

    QState *s2Child = new QState(s2);
    s2Child->setPropertyOnEntry(object, "bar", 10.0);
    s2->setInitialState(s2Child);

    QState *s2Child2 = new QState(s2);
    s2Child2->setPropertyOnEntry(object, "bar", 11.0);
    s2Child->addTransition(s2Child2); // should *not* be considered by QAnimationState as part of target

    QAnimationState *as = s1->addAnimatedTransition(new EventTransition(QEvent::User, s2));
    as->addAnimation(new QPropertyAnimation(object, "foo", as));
    as->addAnimation(new QPropertyAnimation(object, "bar", as));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));

    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);

    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 2.0);
    QCOMPARE(s2->valueOnEntry.at(1).toDouble(), 10.0);
    QCOMPARE(object->property("bar").toDouble(), 11.0);
}

void tst_QAnimationState::reuseAnimation()
{
   QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);

    QState *s1 = new QState(machine.rootState());
    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->setPropertyOnEntry(object, "foo", 5.0);
    ValueCheckerState *s3 = new ValueCheckerState(machine.rootState());
    s3->setPropertyOnEntry(object, "foo", 2.0);
    s3->addPropertyToCheck(object, "foo");

    QPropertyAnimation *anim = new QPropertyAnimation(object, "foo", s2);
    s1->addAnimatedTransition(new EventTransition(QEvent::User, s2), anim);
    s2->addAnimatedTransition(new EventTransition(QEvent::User, s3), anim);

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 5.0);

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s3->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s3->valueOnEntry.at(0).toDouble(), 2.0);
}

void tst_QAnimationState::globalRestoreProperty()
{
    QStateMachine machine;
    machine.setGlobalRestorePolicy(QState::RestoreProperties);

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);
    object->setProperty("bar", 3.0);

    QState *s1 = new QState(machine.rootState());

    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->addPropertyToCheck(object, "bar");
    s2->setPropertyOnEntry(object, "foo", 2.0);

    ValueCheckerState *s3 = new ValueCheckerState(machine.rootState());
    s3->addPropertyToCheck(object, "foo");
    s3->addPropertyToCheck(object, "bar");
    s3->setPropertyOnEntry(object, "bar", 5.0);

    ValueCheckerState *s4 = new ValueCheckerState(machine.rootState());
    s4->addPropertyToCheck(object, "foo");
    s4->addPropertyToCheck(object, "bar");

    QAnimationState *as = s1->addAnimatedTransition(new EventTransition(QEvent::User, s2));
    as->addAnimation(new QPropertyAnimation(object, "foo", as));
    as->addAnimation(new QPropertyAnimation(object, "bar", as));

    as = s2->addAnimatedTransition(new EventTransition(QEvent::User, s3));
    as->addAnimation(new QPropertyAnimation(object, "foo", as));
    as->addAnimation(new QPropertyAnimation(object, "bar", as));

    as = s3->addAnimatedTransition(new EventTransition(QEvent::User, s4));
    as->addAnimation(new QPropertyAnimation(object, "foo", as));
    as->addAnimation(new QPropertyAnimation(object, "bar", as));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 2.0);
    QCOMPARE(s2->valueOnEntry.at(1).toDouble(), 3.0);

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s3->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s3->valueOnEntry.at(0).toDouble(), 1.0);
    QCOMPARE(s3->valueOnEntry.at(1).toDouble(), 5.0);

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s4->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s4->valueOnEntry.at(0).toDouble(), 1.0);
    QCOMPARE(s4->valueOnEntry.at(1).toDouble(), 3.0);

}

void tst_QAnimationState::specificRestoreProperty()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);
    object->setProperty("bar", 3.0);

    QState *s1 = new QState(machine.rootState());

    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->setRestorePolicy(QState::RestoreProperties);
    s2->addPropertyToCheck(object, "foo");
    s2->addPropertyToCheck(object, "bar");
    s2->setPropertyOnEntry(object, "foo", 2.0);

    ValueCheckerState *s3 = new ValueCheckerState(machine.rootState());
    s3->setRestorePolicy(QState::RestoreProperties);
    s3->addPropertyToCheck(object, "foo");
    s3->addPropertyToCheck(object, "bar");
    s3->setPropertyOnEntry(object, "bar", 5.0);

    ValueCheckerState *s4 = new ValueCheckerState(machine.rootState());
    s4->setRestorePolicy(QState::RestoreProperties);
    s4->addPropertyToCheck(object, "foo");
    s4->addPropertyToCheck(object, "bar");

    QAnimationState *as = s1->addAnimatedTransition(new EventTransition(QEvent::User, s2));
    as->addAnimation(new QPropertyAnimation(object, "foo", as));

    as = s2->addAnimatedTransition(new EventTransition(QEvent::User, s3));
    as->addAnimation(new QPropertyAnimation(object, "bar", as));

    as = s3->addAnimatedTransition(new EventTransition(QEvent::User, s4));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 2.0);
    QCOMPARE(s2->valueOnEntry.at(1).toDouble(), 3.0);

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s3->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s3->valueOnEntry.at(0).toDouble(), 1.0);
    QCOMPARE(s3->valueOnEntry.at(1).toDouble(), 5.0);

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s4->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s4->valueOnEntry.at(0).toDouble(), 1.0);
    QCOMPARE(s4->valueOnEntry.at(1).toDouble(), 3.0);
}

void tst_QAnimationState::someAnimationsNotSpecified()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);
    object->setProperty("bar", 3.0);

    QState *s1 = new QState(machine.rootState());

    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->addPropertyToCheck(object, "bar");
    s2->setPropertyOnEntry(object, "foo", 2.0);

    QAnimationState *as = s1->addAnimatedTransition(new EventTransition(QEvent::User, s2));
    as->addAnimation(new QPropertyAnimation(object, "foo", as));
    as->addAnimation(new QPropertyAnimation(object, "bar", as));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 2.0);
    QCOMPARE(s2->valueOnEntry.at(1).toDouble(), 3.0);
}

void tst_QAnimationState::someActionsNotAnimated()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);
    object->setProperty("bar", 3.0);

    QState *s1 = new QState(machine.rootState());

    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->addPropertyToCheck(object, "bar");
    s2->setPropertyOnEntry(object, "foo", 2.0);
    s2->setPropertyOnEntry(object, "bar", 5.0);

    s1->addAnimatedTransition(new EventTransition(QEvent::User, s2),
                              new QPropertyAnimation(object, "foo", s1));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 2.0);
    QCOMPARE(s2->valueOnEntry.at(1).toDouble(), 3.0);
    QCOMPARE(object->property("foo").toDouble(), 2.0);
    QCOMPARE(object->property("bar").toDouble(), 5.0);
}

void tst_QAnimationState::specificTargetValueOfAnimation()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);

    QState *s1 = new QState(machine.rootState());

    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->setPropertyOnEntry(object, "foo", 2.0);

    QPropertyAnimation *anim = new QPropertyAnimation(object, "foo");
    anim->setEndValue(10.0);
    s1->addAnimatedTransition(new EventTransition(QEvent::User, s2), anim);

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 10.0);
    QCOMPARE(object->property("foo").toDouble(), 2.0);
}

void tst_QAnimationState::persistentTargetValueOfAnimation()
{
    QStateMachine machine;

    QObject *object = new QObject();
    object->setProperty("foo", 1.0);

    QState *s1 = new QState(machine.rootState());

    ValueCheckerState *s2 = new ValueCheckerState(machine.rootState());
    s2->addPropertyToCheck(object, "foo");
    s2->setPropertyOnEntry(object, "foo", 2.0);

    QPropertyAnimation *anim = new QPropertyAnimation(object, "foo");
    s1->addAnimatedTransition(new EventTransition(QEvent::User, s2), anim);

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QTRY_COMPARE(s2->valueOnEntry.at(0).isValid(), true);
    QCOMPARE(s2->valueOnEntry.at(0).toDouble(), 2.0);
    QCOMPARE(anim->endValue().isValid(), false);
}


QTEST_MAIN(tst_QAnimationState)
#include "tst_qanimationstate.moc"
