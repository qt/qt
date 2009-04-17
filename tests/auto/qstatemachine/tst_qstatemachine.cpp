/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtGui/QPushButton>

#include "qstatemachine.h"
#include "qstate.h"
#include "qtransition.h"
#include "qhistorystate.h"
#include "qkeyeventtransition.h"
#include "qmouseeventtransition.h"
#include "private/qstatemachine_p.h"

// Will try to wait for the condition while allowing event processing
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

//TESTED_CLASS=
//TESTED_FILES=

static int globalTick;

class tst_QStateMachine : public QObject
{
    Q_OBJECT
public:
    tst_QStateMachine();
    virtual ~tst_QStateMachine();

private slots:
    void init();
    void cleanup();

    void rootState();
    void addAndRemoveState();
    void stateEntryAndExit();
    void postEvent();
    void stateFinished();
    void parallelStates();
    void allSourceToTargetConfigurations();
    void signalTransitions();
    void eventTransitions();
    void historyStates();
    void stateActions();
    void transitionActions();
    void transitionToRootState();
    void transitionEntersParent();
    
    void defaultErrorState();
    void customGlobalErrorState();
    void customLocalErrorStateInBrokenState();
    void customLocalErrorStateInOtherState();
    void customLocalErrorStateInParentOfBrokenState();
    void customLocalErrorStateOverridesParent();
    void errorStateHasChildren();
    void errorStateHasErrors();
    void errorStateIsRootState();
    void errorStateEntersParentFirst();
    void customErrorStateIsNull();
    void clearError();
    void historyStateHasNowhereToGo();
    void brokenStateIsNeverEntered();
    void customErrorStateNotInGraph();
    void transitionToStateNotInGraph();
    void restoreProperties();
    void defaultGlobalRestorePolicy();
    void globalRestorePolicySetToRestore();
    void globalRestorePolicySetToDoNotRestore();
    void restorePolicyNotInherited();
    void mixedRestoreProperties();
    void setRestorePolicyToDoNotRestore();
    void setGlobalRestorePolicyToGlobalRestore();
    void restorePolicyOnChildState();
    void transitionWithParent();
};

tst_QStateMachine::tst_QStateMachine()
{
}

tst_QStateMachine::~tst_QStateMachine()
{
}

class TestState : public QState
{
public:
    enum Event {
        Entry,
        Exit
    };
    TestState(QState *parent)
        : QState(parent) {}
    QList<QPair<int, Event> > events;
protected:
    virtual void onEntry() {
        events.append(qMakePair(globalTick++, Entry));
    }
    virtual void onExit() {
        events.append(qMakePair(globalTick++, Exit));
    }
};

class TestTransition : public QAbstractTransition
{
public:
    TestTransition(QAbstractState *target)
        : QAbstractTransition(QList<QAbstractState*>() << target) {}
    QList<int> triggers;
protected:
    virtual bool eventTest(QEvent *) const {
        return true;
    }
    virtual void onTransition() {
        triggers.append(globalTick++);
    }
};

static QtMsgType s_msgType;
static QByteArray s_msg;
static bool s_countWarnings;
static QtMsgHandler s_oldHandler;

static void defaultErrorStateTestMessageHandler(QtMsgType type, const char *msg)
{    
    s_msgType = type;
    s_msg = msg;
    
    if (s_countWarnings)
        s_oldHandler(type, msg);
}

void tst_QStateMachine::init()
{
    s_msg = QByteArray();
    s_msgType = QtDebugMsg;
    s_countWarnings = true;

    s_oldHandler = qInstallMsgHandler(defaultErrorStateTestMessageHandler);
}

void tst_QStateMachine::cleanup()
{
    qInstallMsgHandler(s_oldHandler);
}

class EventTransition : public QTransition
{
public:
    EventTransition(QEvent::Type type, QAbstractState *target, QState *parent = 0)
        : QTransition(QList<QAbstractState*>() << target, parent), m_type(type) {}
protected:
    virtual bool eventTest(QEvent *e) const {
        return (e->type() == m_type);
    }
private:
    QEvent::Type m_type;
};

void tst_QStateMachine::transitionToRootState()
{
    QStateMachine machine;

    QState *initialState = new QState();
    machine.addState(initialState);
    machine.setInitialState(initialState);

    initialState->addTransition(new EventTransition(QEvent::User, machine.rootState()));

    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 2);
    QVERIFY(machine.configuration().contains(initialState));
    QVERIFY(machine.configuration().contains(machine.rootState()));

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 2);
    QVERIFY(machine.configuration().contains(initialState));
    QVERIFY(machine.configuration().contains(machine.rootState()));
}

void tst_QStateMachine::transitionEntersParent()
{
    QStateMachine machine;

    QObject *entryController = new QObject(&machine);
    entryController->setObjectName("entryController");
    entryController->setProperty("greatGrandParentEntered", false);
    entryController->setProperty("grandParentEntered", false);
    entryController->setProperty("parentEntered", false);
    entryController->setProperty("stateEntered", false);

    QState *greatGrandParent = new QState();
    greatGrandParent->setObjectName("grandParent");
    greatGrandParent->setPropertyOnEntry(entryController, "greatGrandParentEntered", true);
    machine.addState(greatGrandParent);
    machine.setInitialState(greatGrandParent);

    QState *grandParent = new QState(greatGrandParent);
    grandParent->setObjectName("grandParent");
    grandParent->setPropertyOnEntry(entryController, "grandParentEntered", true);

    QState *parent = new QState(grandParent);
    parent->setObjectName("parent");
    parent->setPropertyOnEntry(entryController, "parentEntered", true);

    QState *state = new QState(parent);
    state->setObjectName("state");
    state->setPropertyOnEntry(entryController, "stateEntered", true);

    QState *initialStateOfGreatGrandParent = new QState(greatGrandParent);
    initialStateOfGreatGrandParent->setObjectName("initialStateOfGreatGrandParent");
    greatGrandParent->setInitialState(initialStateOfGreatGrandParent);

    initialStateOfGreatGrandParent->addTransition(new EventTransition(QEvent::User, state));

    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(entryController->property("greatGrandParentEntered").toBool(), true);
    QCOMPARE(entryController->property("grandParentEntered").toBool(), false);
    QCOMPARE(entryController->property("parentEntered").toBool(), false);
    QCOMPARE(entryController->property("stateEntered").toBool(), false);
    QCOMPARE(machine.configuration().count(), 2);
    QVERIFY(machine.configuration().contains(greatGrandParent));
    QVERIFY(machine.configuration().contains(initialStateOfGreatGrandParent));

    entryController->setProperty("greatGrandParentEntered", false);
    entryController->setProperty("grandParentEntered", false);
    entryController->setProperty("parentEntered", false);
    entryController->setProperty("stateEntered", false);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(entryController->property("greatGrandParentEntered").toBool(), false);
    QCOMPARE(entryController->property("grandParentEntered").toBool(), true);
    QCOMPARE(entryController->property("parentEntered").toBool(), true);
    QCOMPARE(entryController->property("stateEntered").toBool(), true);
    QCOMPARE(machine.configuration().count(), 4);
    QVERIFY(machine.configuration().contains(greatGrandParent));
    QVERIFY(machine.configuration().contains(grandParent));
    QVERIFY(machine.configuration().contains(parent));
    QVERIFY(machine.configuration().contains(state));
}

void tst_QStateMachine::defaultErrorState()
{
    s_countWarnings = false; // we expect warnings here 

    QStateMachine machine;
    QVERIFY(machine.errorState() != 0);

    QState *brokenState = new QState();
    brokenState->setObjectName("MyInitialState");

    machine.addState(brokenState);
    machine.setInitialState(brokenState);

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    // initialState has no initial state
    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(s_msgType, QtWarningMsg);
    QCOMPARE(QString::fromLatin1(s_msg.data()), 
             QString::fromLatin1("Unrecoverable error detected in running state machine: Missing initial state in compound state 'MyInitialState'"));

    QCOMPARE(machine.error(), QStateMachine::NoInitialStateError);
    QCOMPARE(machine.errorString(), QString::fromLatin1("Missing initial state in compound state 'MyInitialState'"));

    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(machine.errorState()));
}

class CustomErrorState: public QState
{
public:
    CustomErrorState(QStateMachine *machine, QState *parent = 0) 
        : QState(parent), error(QStateMachine::NoError), m_machine(machine)
    {
    }

    void onEntry() 
    {
        error = m_machine->error();        
        errorString = m_machine->errorString();
    }

    QStateMachine::Error error;
    QString errorString;

private:
    QStateMachine *m_machine;
};

void tst_QStateMachine::customGlobalErrorState()
{
    QStateMachine machine;

    CustomErrorState *customErrorState = new CustomErrorState(&machine);
    customErrorState->setObjectName("customErrorState");
    machine.addState(customErrorState);
    machine.setErrorState(customErrorState);

    QState *initialState = new QState();
    initialState->setObjectName("initialState");
    machine.addState(initialState);
    machine.setInitialState(initialState);

    QState *brokenState = new QState();
    brokenState->setObjectName("brokenState");
    machine.addState(brokenState);
    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialState->addTransition(new EventTransition(QEvent::Type(QEvent::User + 1), brokenState));
    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(machine.errorState(), customErrorState);
    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(initialState));

    machine.postEvent(new QEvent(QEvent::Type(QEvent::User + 1)));
    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(initialState));

    QCoreApplication::processEvents();
    
    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(customErrorState));
    QCOMPARE(customErrorState->error, QStateMachine::NoInitialStateError);
    QCOMPARE(customErrorState->errorString, QString::fromLatin1("Missing initial state in compound state 'brokenState'"));
    QCOMPARE(machine.error(), QStateMachine::NoInitialStateError);
    QCOMPARE(machine.errorString(), QString::fromLatin1("Missing initial state in compound state 'brokenState'"));
    QVERIFY(s_msg.isEmpty());
    QCOMPARE(s_msgType, QtDebugMsg);
}

void tst_QStateMachine::customLocalErrorStateInBrokenState()
{    
    QStateMachine machine;
    CustomErrorState *customErrorState = new CustomErrorState(&machine);
    machine.addState(customErrorState);

    QState *initialState = new QState();
    initialState->setObjectName("initialState");
    machine.addState(initialState);
    machine.setInitialState(initialState);

    QState *brokenState = new QState();
    brokenState->setObjectName("brokenState");
    machine.addState(brokenState);    
    brokenState->setErrorState(customErrorState);    

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialState->addTransition(new EventTransition(QEvent::Type(QEvent::User + 1), brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(customErrorState));
    QCOMPARE(customErrorState->error, QStateMachine::NoInitialStateError);
    QVERIFY(s_msg.isEmpty());
}

void tst_QStateMachine::customLocalErrorStateInOtherState()
{
    s_countWarnings = false;

    QStateMachine machine;
    CustomErrorState *customErrorState = new CustomErrorState(&machine);
    machine.addState(customErrorState);

    QState *initialState = new QState();
    initialState->setObjectName("initialState");
    initialState->setErrorState(customErrorState);
    machine.addState(initialState);
    machine.setInitialState(initialState);

    QState *brokenState = new QState();
    brokenState->setObjectName("brokenState");
    
    machine.addState(brokenState);    

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialState->addTransition(new EventTransition(QEvent::Type(QEvent::User + 1), brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(machine.errorState()));
    QCOMPARE(s_msgType, QtWarningMsg);
}

void tst_QStateMachine::customLocalErrorStateInParentOfBrokenState()
{
    QStateMachine machine;
    CustomErrorState *customErrorState = new CustomErrorState(&machine);
    machine.addState(customErrorState);

    QState *initialState = new QState();
    initialState->setObjectName("initialState");
    machine.addState(initialState);
    machine.setInitialState(initialState);

    QState *parentOfBrokenState = new QState();
    machine.addState(parentOfBrokenState);
    parentOfBrokenState->setObjectName("parentOfBrokenState");
    parentOfBrokenState->setErrorState(customErrorState);
    

    QState *brokenState = new QState(parentOfBrokenState);
    brokenState->setObjectName("brokenState");
    parentOfBrokenState->setInitialState(brokenState);

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialState->addTransition(new EventTransition(QEvent::Type(QEvent::User + 1), brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(customErrorState));
    QVERIFY(s_msg.isEmpty());
}

void tst_QStateMachine::customLocalErrorStateOverridesParent()
{
    QStateMachine machine;
    CustomErrorState *customErrorStateForParent = new CustomErrorState(&machine);
    machine.addState(customErrorStateForParent);

    CustomErrorState *customErrorStateForBrokenState = new CustomErrorState(&machine);
    machine.addState(customErrorStateForBrokenState);

    QState *initialState = new QState();
    initialState->setObjectName("initialState");
    machine.addState(initialState);
    machine.setInitialState(initialState);

    QState *parentOfBrokenState = new QState();
    machine.addState(parentOfBrokenState);
    parentOfBrokenState->setObjectName("parentOfBrokenState");
    parentOfBrokenState->setErrorState(customErrorStateForParent);
    
    QState *brokenState = new QState(parentOfBrokenState);
    brokenState->setObjectName("brokenState");
    brokenState->setErrorState(customErrorStateForBrokenState);
    parentOfBrokenState->setInitialState(brokenState);

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialState->addTransition(new EventTransition(QEvent::Type(QEvent::User + 1), brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(customErrorStateForBrokenState));
    QCOMPARE(customErrorStateForBrokenState->error, QStateMachine::NoInitialStateError);
    QCOMPARE(customErrorStateForParent->error, QStateMachine::NoError);
    QVERIFY(s_msg.isEmpty());
}

void tst_QStateMachine::errorStateHasChildren()
{
    QStateMachine machine;
    CustomErrorState *customErrorState = new CustomErrorState(&machine);
    customErrorState->setObjectName("customErrorState");
    machine.addState(customErrorState);

    machine.setErrorState(customErrorState);

    QState *childOfErrorState = new QState(customErrorState);
    childOfErrorState->setObjectName("childOfErrorState");
    customErrorState->setInitialState(childOfErrorState);

    QState *initialState = new QState();
    initialState->setObjectName("initialState");
    machine.addState(initialState);
    machine.setInitialState(initialState);    

    QState *brokenState = new QState();
    brokenState->setObjectName("brokenState");    
    machine.addState(brokenState);

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialState->addTransition(new EventTransition(QEvent::Type(QEvent::User + 1), brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 2);
    QVERIFY(machine.configuration().contains(customErrorState));
    QVERIFY(machine.configuration().contains(childOfErrorState)); 
    QVERIFY(s_msg.isEmpty());
}


void tst_QStateMachine::errorStateHasErrors()
{
    s_countWarnings = false;

    QStateMachine machine;
    CustomErrorState *customErrorState = new CustomErrorState(&machine);
    customErrorState->setObjectName("customErrorState");
    machine.addState(customErrorState);

    QAbstractState *oldErrorState = machine.errorState();
    machine.setErrorState(customErrorState);

    QState *childOfErrorState = new QState(customErrorState);
    childOfErrorState->setObjectName("childOfErrorState");

    QState *initialState = new QState();
    initialState->setObjectName("initialState");
    machine.addState(initialState);
    machine.setInitialState(initialState);    

    QState *brokenState = new QState();
    brokenState->setObjectName("brokenState");    
    machine.addState(brokenState);

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialState->addTransition(new EventTransition(QEvent::Type(QEvent::User + 1), brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(oldErrorState)); // Fall back to default
    QCOMPARE(machine.error(), QStateMachine::NoInitialStateError);
    QCOMPARE(machine.errorString(), QString::fromLatin1("Missing initial state in compound state 'customErrorState'"));

    QCOMPARE(s_msgType, QtWarningMsg);
}

void tst_QStateMachine::errorStateIsRootState()
{
    QStateMachine machine;
    machine.setErrorState(machine.rootState());

    QState *initialState = new QState();
    initialState->setObjectName("initialState");
    machine.addState(initialState);
    machine.setInitialState(initialState);    
    
    QState *brokenState = new QState();
    brokenState->setObjectName("brokenState");    
    machine.addState(brokenState);

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialState->addTransition(new EventTransition(QEvent::Type(QEvent::User + 1), brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::processEvents();

    QEXPECT_FAIL("", "Covered by transitionToRootState test. Remove this when that test passes", Continue);
    QCOMPARE(machine.configuration().count(), 1);    
    QVERIFY(machine.configuration().contains(initialState));
}

void tst_QStateMachine::errorStateEntersParentFirst()
{
    QStateMachine machine;

    QObject *entryController = new QObject(&machine);
    entryController->setObjectName("entryController");
    entryController->setProperty("greatGrandParentEntered", false);
    entryController->setProperty("grandParentEntered", false);
    entryController->setProperty("parentEntered", false);
    entryController->setProperty("errorStateEntered", false);

    QState *greatGrandParent = new QState();
    greatGrandParent->setObjectName("greatGrandParent");
    greatGrandParent->setPropertyOnEntry(entryController, "greatGrandParentEntered", true);
    machine.addState(greatGrandParent);
    machine.setInitialState(greatGrandParent);

    QState *grandParent = new QState(greatGrandParent);
    grandParent->setObjectName("grandParent");
    grandParent->setPropertyOnEntry(entryController, "grandParentEntered", true);    

    QState *parent = new QState(grandParent);
    parent->setObjectName("parent");
    parent->setPropertyOnEntry(entryController, "parentEntered", true);

    QState *errorState = new QState(parent);
    errorState->setObjectName("errorState");
    errorState->setPropertyOnEntry(entryController, "errorStateEntered", true);
    machine.setErrorState(errorState);

    QState *initialStateOfGreatGrandParent = new QState(greatGrandParent);
    initialStateOfGreatGrandParent->setObjectName("initialStateOfGreatGrandParent");
    greatGrandParent->setInitialState(initialStateOfGreatGrandParent);

    QState *brokenState = new QState(greatGrandParent);    
    brokenState->setObjectName("brokenState");

    QState *childState = new QState(brokenState);
    childState->setObjectName("childState");

    initialStateOfGreatGrandParent->addTransition(new EventTransition(QEvent::User, brokenState));

    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(entryController->property("greatGrandParentEntered").toBool(), true);
    QCOMPARE(entryController->property("grandParentEntered").toBool(), false);
    QCOMPARE(entryController->property("parentEntered").toBool(), false);
    QCOMPARE(entryController->property("errorStateEntered").toBool(), false);
    QCOMPARE(machine.configuration().count(), 2);
    QVERIFY(machine.configuration().contains(greatGrandParent));
    QVERIFY(machine.configuration().contains(initialStateOfGreatGrandParent));

    entryController->setProperty("greatGrandParentEntered", false);
    entryController->setProperty("grandParentEntered", false);
    entryController->setProperty("parentEntered", false);
    entryController->setProperty("errorStateEntered", false);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(entryController->property("greatGrandParentEntered").toBool(), false);
    QCOMPARE(entryController->property("grandParentEntered").toBool(), true);
    QCOMPARE(entryController->property("parentEntered").toBool(), true);
    QCOMPARE(entryController->property("errorStateEntered").toBool(), true);
    QCOMPARE(machine.configuration().count(), 4);
    QVERIFY(machine.configuration().contains(greatGrandParent));
    QVERIFY(machine.configuration().contains(grandParent));
    QVERIFY(machine.configuration().contains(parent));
    QVERIFY(machine.configuration().contains(errorState));
}

void tst_QStateMachine::customErrorStateIsNull()
{
    s_countWarnings = false;

    QStateMachine machine;
    QAbstractState *oldErrorState = machine.errorState();
    machine.rootState()->setErrorState(0);

    QState *initialState = new QState();
    machine.addState(initialState);
    machine.setInitialState(initialState);

    QState *brokenState = new QState();
    machine.addState(brokenState);

    new QState(brokenState);
    initialState->addTransition(new EventTransition(QEvent::User, brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(machine.errorState(), reinterpret_cast<void *>(0));
    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(oldErrorState));
    QCOMPARE(s_msgType, QtWarningMsg);
}

void tst_QStateMachine::clearError()
{
    QStateMachine machine;    
    machine.setErrorState(new QState(machine.rootState())); // avoid warnings
    
    QState *brokenState = new QState(machine.rootState());
    brokenState->setObjectName("brokenState");
    machine.setInitialState(brokenState);
    new QState(brokenState);

    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(machine.error(), QStateMachine::NoInitialStateError);
    QCOMPARE(machine.errorString(), QString::fromLatin1("Missing initial state in compound state 'brokenState'"));

    machine.clearError();

    QCOMPARE(machine.error(), QStateMachine::NoError);
    QVERIFY(machine.errorString().isEmpty());
}

void tst_QStateMachine::historyStateHasNowhereToGo()
{
    QStateMachine machine;

    QState *initialState = new QState(machine.rootState());
    machine.setInitialState(initialState);
    machine.setErrorState(new QState(machine.rootState())); // avoid warnings

    QState *brokenState = new QState(machine.rootState());
    brokenState->setObjectName("brokenState");
    brokenState->setInitialState(new QState(brokenState));

    QHistoryState *historyState = brokenState->addHistoryState();
    historyState->setObjectName("historyState");
    initialState->addTransition(new EventTransition(QEvent::User, historyState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(machine.errorState()));
    QCOMPARE(machine.error(), QStateMachine::NoDefaultStateInHistoryState);
    QCOMPARE(machine.errorString(), QString::fromLatin1("Missing transition from history state 'historyState'"));
}

void tst_QStateMachine::brokenStateIsNeverEntered()
{
    QStateMachine machine;

    QObject *entryController = new QObject();
    entryController->setProperty("brokenStateEntered", false);
    entryController->setProperty("childStateEntered", false);
    entryController->setProperty("errorStateEntered", false);

    QState *initialState = new QState(machine.rootState());    
    machine.setInitialState(initialState);

    QState *errorState = new QState(machine.rootState());
    errorState->setPropertyOnEntry(entryController, "errorStateEntered", true);
    machine.setErrorState(errorState); 

    QState *brokenState = new QState(machine.rootState());
    brokenState->setPropertyOnEntry(entryController, "brokenStateEntered", true);
    brokenState->setObjectName("brokenState");
    
    QState *childState = new QState(brokenState);
    childState->setPropertyOnEntry(entryController, "childStateEntered", true);

    initialState->addTransition(new EventTransition(QEvent::User, brokenState));

    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(entryController->property("errorStateEntered").toBool(), true);
    QCOMPARE(entryController->property("brokenStateEntered").toBool(), false);
    QCOMPARE(entryController->property("childStateEntered").toBool(), false);
}

void tst_QStateMachine::transitionToStateNotInGraph()
{
    QSKIP("Hangs", SkipAll);

    s_countWarnings = false;

    QStateMachine machine;

    QState *initialState = new QState(machine.rootState());
    initialState->setObjectName("initialState");
    machine.setInitialState(initialState);

    QState *independentState = new QState();
    initialState->addTransition(independentState);

    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(initialState));
}

void tst_QStateMachine::customErrorStateNotInGraph()
{
    s_countWarnings = false;

    QStateMachine machine;

    QState *errorState = new QState();
    errorState->setObjectName("errorState");
    machine.setErrorState(errorState);
    QVERIFY(errorState != machine.errorState());

    QState *initialBrokenState = new QState(machine.rootState());
    initialBrokenState->setObjectName("initialBrokenState");
    machine.setInitialState(initialBrokenState);
    new QState(initialBrokenState);

    machine.start();
    QCoreApplication::processEvents();
    
    QCOMPARE(machine.configuration().count(), 1);
    QVERIFY(machine.configuration().contains(machine.errorState()));        
}

void tst_QStateMachine::restoreProperties()
{
    QObject *object = new QObject();
    object->setProperty("a", 1);
    object->setProperty("b", 2);

    QStateMachine machine;

    QState *S1 = new QState();
    S1->setObjectName("S1");
    S1->setPropertyOnEntry(object, "a", 3);
    S1->setRestorePolicy(QState::RestoreProperties);
    machine.addState(S1);

    QState *S2 = new QState();
    S2->setObjectName("S2");
    S2->setPropertyOnEntry(object, "b", 5);
    S2->setRestorePolicy(QState::RestoreProperties);
    machine.addState(S2);

    QState *S3 = new QState();
    S3->setObjectName("S3");
    S3->setRestorePolicy(QState::RestoreProperties);
    machine.addState(S3);

    QFinalState *S4 = new QFinalState();
    machine.addState(S4);

    S1->addTransition(new EventTransition(QEvent::User, S2));
    S2->addTransition(new EventTransition(QEvent::User, S3));
    S3->addTransition(S4);

    machine.setInitialState(S1);
    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(object->property("a").toInt(), 3);
    QCOMPARE(object->property("b").toInt(), 2);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(object->property("a").toInt(), 1);
    QCOMPARE(object->property("b").toInt(), 5);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(object->property("a").toInt(), 1);
    QCOMPARE(object->property("b").toInt(), 2);
}

void tst_QStateMachine::rootState()
{
    QStateMachine machine;
    QVERIFY(machine.rootState() != 0);
    QVERIFY(qobject_cast<QState*>(machine.rootState()) != 0);
    QCOMPARE(qobject_cast<QState*>(machine.rootState())->parentState(), (QState*)0);
    QCOMPARE(machine.rootState()->parent(), (QObject*)&machine);

    QState *s1 = new QState(machine.rootState());
    QCOMPARE(s1->parentState(), machine.rootState());

    QState *s2 = new QState();
    s2->setParent(&machine);
    QCOMPARE(s2->parentState(), machine.rootState());
}

void tst_QStateMachine::addAndRemoveState()
{
    QStateMachine machine;
    QCOMPARE(machine.states().size(), 1); // the error state
    QCOMPARE(machine.states().at(0), (QAbstractState*)machine.errorState());

    QTest::ignoreMessage(QtWarningMsg, "QStateMachine::addState: cannot add null state");
    machine.addState(0);

    QState *s1 = new QState();
    QCOMPARE(s1->parentState(), (QState*)0);
    machine.addState(s1);
    QCOMPARE(s1->parentState(), machine.rootState());
    QCOMPARE(machine.states().size(), 2);
    QCOMPARE(machine.states().at(0), (QAbstractState*)machine.errorState());
    QCOMPARE(machine.states().at(1), (QAbstractState*)s1);

    QTest::ignoreMessage(QtWarningMsg, "QStateMachine::addState: state has already been added to this machine");
    machine.addState(s1);

    QState *s2 = new QState();
    QCOMPARE(s2->parentState(), (QState*)0);
    machine.addState(s2);
    QCOMPARE(s2->parentState(), machine.rootState());
    QCOMPARE(machine.states().size(), 3);
    QCOMPARE(machine.states().at(0), (QAbstractState*)machine.errorState());
    QCOMPARE(machine.states().at(1), (QAbstractState*)s1);
    QCOMPARE(machine.states().at(2), (QAbstractState*)s2);

    QTest::ignoreMessage(QtWarningMsg, "QStateMachine::addState: state has already been added to this machine");
    machine.addState(s2);

    machine.removeState(s1);
    QCOMPARE(s1->parentState(), (QState*)0);
    QCOMPARE(machine.states().size(), 2);
    QCOMPARE(machine.states().at(0), (QAbstractState*)machine.errorState());
    QCOMPARE(machine.states().at(1), (QAbstractState*)s2);

    machine.removeState(s2);
    QCOMPARE(s2->parentState(), (QState*)0);
    QCOMPARE(machine.states().size(), 1);
    QCOMPARE(machine.states().at(0), (QAbstractState*)machine.errorState());

    QTest::ignoreMessage(QtWarningMsg, "QStateMachine::removeState: cannot remove null state");
    machine.removeState(0);

    delete s1;
    delete s2;
    // ### how to deal with this?
    // machine.removeState(machine.errorState());
}

void tst_QStateMachine::stateEntryAndExit()
{
    // Two top-level states
    {
        QStateMachine machine;

        TestState *s1 = new TestState(machine.rootState());
        TestState *s2 = new TestState(machine.rootState());
        QFinalState *s3 = new QFinalState(machine.rootState());
        TestTransition *t = new TestTransition(s2);
        s1->addTransition(t);
        s2->addTransition(s3);

        QSignalSpy startedSpy(&machine, SIGNAL(started()));
        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s1);
        QVERIFY(machine.configuration().isEmpty());
        globalTick = 0;
        machine.start();

        QTRY_COMPARE(startedSpy.count(), 1);
        QTRY_COMPARE(finishedSpy.count(), 1);
        QCOMPARE(machine.configuration().count(), 1);
        QVERIFY(machine.configuration().contains(s3));

        // s1 is entered
        QCOMPARE(s1->events.count(), 2);
        QCOMPARE(s1->events.at(0).first, 0);
        QCOMPARE(s1->events.at(0).second, TestState::Entry);
        // s1 is exited
        QCOMPARE(s1->events.at(1).first, 1);
        QCOMPARE(s1->events.at(1).second, TestState::Exit);
        // t is triggered
        QCOMPARE(t->triggers.count(), 1);
        QCOMPARE(t->triggers.at(0), 2);
        // s2 is entered
        QCOMPARE(s2->events.count(), 2);
        QCOMPARE(s2->events.at(0).first, 3);
        QCOMPARE(s2->events.at(0).second, TestState::Entry);
        // s2 is exited
        QCOMPARE(s2->events.at(1).first, 4);
        QCOMPARE(s2->events.at(1).second, TestState::Exit);
    }
    // Two top-level states, one has two child states
    {
        QStateMachine machine;

        TestState *s1 = new TestState(machine.rootState());
        TestState *s11 = new TestState(s1);
        TestState *s12 = new TestState(s1);
        TestState *s2 = new TestState(machine.rootState());
        QFinalState *s3 = new QFinalState(machine.rootState());
        s1->setInitialState(s11);
        TestTransition *t1 = new TestTransition(s12);
        s11->addTransition(t1);
        TestTransition *t2 = new TestTransition(s2);
        s12->addTransition(t2);
        s2->addTransition(s3);

        QSignalSpy startedSpy(&machine, SIGNAL(started()));
        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s1);
        globalTick = 0;
        machine.start();

        QTRY_COMPARE(startedSpy.count(), 1);
        QTRY_COMPARE(finishedSpy.count(), 1);
        QCOMPARE(machine.configuration().count(), 1);
        QVERIFY(machine.configuration().contains(s3));

        // s1 is entered
        QCOMPARE(s1->events.count(), 2);
        QCOMPARE(s1->events.at(0).first, 0);
        QCOMPARE(s1->events.at(0).second, TestState::Entry);
        // s11 is entered
        QCOMPARE(s11->events.count(), 2);
        QCOMPARE(s11->events.at(0).first, 1);
        QCOMPARE(s11->events.at(0).second, TestState::Entry);
        // s11 is exited
        QCOMPARE(s11->events.at(1).first, 2);
        QCOMPARE(s11->events.at(1).second, TestState::Exit);
        // t1 is triggered
        QCOMPARE(t1->triggers.count(), 1);
        QCOMPARE(t1->triggers.at(0), 3);
        // s12 is entered
        QCOMPARE(s12->events.count(), 2);
        QCOMPARE(s12->events.at(0).first, 4);
        QCOMPARE(s12->events.at(0).second, TestState::Entry);
        // s12 is exited
        QCOMPARE(s12->events.at(1).first, 5);
        QCOMPARE(s12->events.at(1).second, TestState::Exit);
        // s1 is exited
        QCOMPARE(s1->events.at(1).first, 6);
        QCOMPARE(s1->events.at(1).second, TestState::Exit);
        // t2 is triggered
        QCOMPARE(t2->triggers.count(), 1);
        QCOMPARE(t2->triggers.at(0), 7);
        // s2 is entered
        QCOMPARE(s2->events.count(), 2);
        QCOMPARE(s2->events.at(0).first, 8);
        QCOMPARE(s2->events.at(0).second, TestState::Entry);
        // s2 is exited
        QCOMPARE(s2->events.at(1).first, 9);
        QCOMPARE(s2->events.at(1).second, TestState::Exit);
    }
}

struct StringEvent : public QEvent
{
public:
    StringEvent(const QString &val)
        : QEvent(QEvent::Type(QEvent::User+2)),
          value(val) {}

    QString value;
};

class StringTransition : public QAbstractTransition
{
public:
    StringTransition(const QString &value, QAbstractState *target)
        : QAbstractTransition(QList<QAbstractState*>() << target), m_value(value) {}

protected:
    virtual bool eventTest(QEvent *e) const
    {
        if (e->type() != QEvent::Type(QEvent::User+2))
            return false;
        StringEvent *se = static_cast<StringEvent*>(e);
        return (m_value == se->value) && (!m_cond.isValid() || (m_cond.indexIn(m_value) != -1));
    }
    virtual void onTransition() {}

private:
    QString m_value;
    QRegExp m_cond;
};

class StringEventPoster : public QState
{
public:
    StringEventPoster(QStateMachine *machine, const QString &value, QState *parent = 0)
        : QState(parent), m_machine(machine), m_value(value) {}

    void setString(const QString &value)
        { m_value = value; }

protected:
    virtual void onEntry()
    {
        m_machine->postEvent(new StringEvent(m_value));
    }
    virtual void onExit() {}

private:
    QStateMachine *m_machine;
    QString m_value;
};

void tst_QStateMachine::postEvent()
{
    QStateMachine machine;
    StringEventPoster *s1 = new StringEventPoster(&machine, "a");
    QFinalState *s2 = new QFinalState;
    s1->addTransition(new StringTransition("a", s2));
    machine.addState(s1);
    machine.addState(s2);
    machine.setInitialState(s1);
    QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
    machine.start();
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(machine.configuration().size(), 1);
    QVERIFY(machine.configuration().contains(s2));

    s1->setString("b");
    QFinalState *s3 = new QFinalState();
    machine.addState(s3);
    s1->addTransition(new StringTransition("b", s3));
    finishedSpy.clear();
    machine.start();
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(machine.configuration().size(), 1);
    QVERIFY(machine.configuration().contains(s3));
}

void tst_QStateMachine::stateFinished()
{
    QStateMachine machine;
    QState *s1 = new QState(machine.rootState());
    QState *s1_1 = new QState(s1);
    QFinalState *s1_2 = new QFinalState(s1);
    s1_1->addTransition(s1_2);
    s1->setInitialState(s1_1);
    QFinalState *s2 = new QFinalState(machine.rootState());
    s1->addTransition(new QStateFinishedTransition(s1, QList<QAbstractState*>() << s2));
    machine.setInitialState(s1);
    QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
    machine.start();
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(machine.configuration().size(), 1);
    QVERIFY(machine.configuration().contains(s2));
}

void tst_QStateMachine::parallelStates()
{
    QStateMachine machine;

    QState *s1 = new QState(QState::ParallelGroup);
      QState *s1_1 = new QState(s1);
        QState *s1_1_1 = new QState(s1_1);
        QFinalState *s1_1_f = new QFinalState(s1_1);
        s1_1_1->addTransition(s1_1_f);
      s1_1->setInitialState(s1_1_1);
      QState *s1_2 = new QState(s1);
        QState *s1_2_1 = new QState(s1_2);
        QFinalState *s1_2_f = new QFinalState(s1_2);
        s1_2_1->addTransition(s1_2_f);
      s1_2->setInitialState(s1_2_1);
    machine.addState(s1);

    QFinalState *s2 = new QFinalState();
    machine.addState(s2);

    s1->addFinishedTransition(s2);

    machine.setInitialState(s1);
    QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
    machine.start();
    QTRY_COMPARE(finishedSpy.count(), 1);
}

void tst_QStateMachine::allSourceToTargetConfigurations()
{
    QStateMachine machine;
    QState *s0 = new QState(machine.rootState());
    s0->setObjectName("s0");
    QState *s1 = new QState(s0);
    s1->setObjectName("s1");
    QState *s11 = new QState(s1);
    s11->setObjectName("s11");
    QState *s2 = new QState(s0);
    s2->setObjectName("s2");
    QState *s21 = new QState(s2);
    s21->setObjectName("s21");
    QState *s211 = new QState(s21);
    s211->setObjectName("s211");
    QFinalState *f = new QFinalState(machine.rootState());
    f->setObjectName("f");

    s0->setInitialState(s1);
    s1->setInitialState(s11);
    s2->setInitialState(s21);
    s21->setInitialState(s211);

    s11->addTransition(new StringTransition("g", s211));
    s1->addTransition(new StringTransition("a", s1));
    s1->addTransition(new StringTransition("b", s11));
    s1->addTransition(new StringTransition("c", s2));
    s1->addTransition(new StringTransition("d", s0));
    s1->addTransition(new StringTransition("f", s211));
    s211->addTransition(new StringTransition("d", s21));
    s211->addTransition(new StringTransition("g", s0));
    s211->addTransition(new StringTransition("h", f));
    s21->addTransition(new StringTransition("b", s211));
    s2->addTransition(new StringTransition("c", s1));
    s2->addTransition(new StringTransition("f", s11));
    s0->addTransition(new StringTransition("e", s211));

    QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
    machine.setInitialState(s0);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new StringEvent("a"));
    QCoreApplication::processEvents();
    machine.postEvent(new StringEvent("b"));
    QCoreApplication::processEvents();
    machine.postEvent(new StringEvent("c"));
    QCoreApplication::processEvents();
    machine.postEvent(new StringEvent("d"));
    QCoreApplication::processEvents();
    machine.postEvent(new StringEvent("e"));
    QCoreApplication::processEvents();
    machine.postEvent(new StringEvent("f"));
    QCoreApplication::processEvents();
    machine.postEvent(new StringEvent("g"));
    QCoreApplication::processEvents();
    machine.postEvent(new StringEvent("h"));
    QCoreApplication::processEvents();

    QTRY_COMPARE(finishedSpy.count(), 1);
}

class SignalEmitter : public QObject
{
Q_OBJECT
    public:
    SignalEmitter(QObject *parent = 0)
        : QObject(parent) {}
    void emitSignalWithNoArg()
        { emit signalWithNoArg(); }
    void emitSignalWithIntArg(int arg)
        { emit signalWithIntArg(arg); }
    void emitSignalWithStringArg(const QString &arg)
        { emit signalWithStringArg(arg); }
Q_SIGNALS:
    void signalWithNoArg();
    void signalWithIntArg(int);
    void signalWithStringArg(const QString &);
};

class TestSignalTransition : public QSignalTransition
{
public:
    TestSignalTransition()
        : QSignalTransition() {}
    TestSignalTransition(QObject *sender, const char *signal,
                         QAbstractState *target)
        : QSignalTransition(sender, signal, QList<QAbstractState*>() << target) {}
    QVariantList argumentsReceived() const {
        return m_args;
    }
protected:
    bool eventTest(QEvent *e) const {
        if (!QSignalTransition::eventTest(e))
            return false;
        QSignalEvent *se = static_cast<QSignalEvent*>(e);
        const_cast<TestSignalTransition*>(this)->m_args = se->arguments();
        return true;
    }
private:
    QVariantList m_args;
};

void tst_QStateMachine::signalTransitions()
{
    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());
        SignalEmitter emitter;
        s0->addTransition(&emitter, SIGNAL(signalWithNoArg()), s1);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        emitter.emitSignalWithNoArg();

        QTRY_COMPARE(finishedSpy.count(), 1);
    }
    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());
        SignalEmitter emitter;
        TestSignalTransition *trans = new TestSignalTransition(&emitter, SIGNAL(signalWithIntArg(int)), s1);
        s0->addTransition(trans);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        emitter.emitSignalWithIntArg(123);

        QTRY_COMPARE(finishedSpy.count(), 1);
        QCOMPARE(trans->argumentsReceived().size(), 1);
        QCOMPARE(trans->argumentsReceived().at(0).toInt(), 123);
    }
    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());
        SignalEmitter emitter;
        TestSignalTransition *trans = new TestSignalTransition(&emitter, SIGNAL(signalWithStringArg(QString)), s1);
        s0->addTransition(trans);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        QString testString = QString::fromLatin1("hello");
        emitter.emitSignalWithStringArg(testString);

        QTRY_COMPARE(finishedSpy.count(), 1);
        QCOMPARE(trans->argumentsReceived().size(), 1);
        QCOMPARE(trans->argumentsReceived().at(0).toString(), testString);
    }
    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());

        TestSignalTransition *trans = new TestSignalTransition();
        QCOMPARE(trans->senderObject(), (QObject*)0);
        QCOMPARE(trans->signal(), QByteArray());

        SignalEmitter emitter;
        trans->setSenderObject(&emitter);
        QCOMPARE(trans->senderObject(), (QObject*)&emitter);
        trans->setSignal(SIGNAL(signalWithNoArg()));
        QCOMPARE(trans->signal(), QByteArray(SIGNAL(signalWithNoArg())));
        trans->setTargetState(s1);
        s0->addTransition(trans);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        emitter.emitSignalWithNoArg();

        QTRY_COMPARE(finishedSpy.count(), 1);
    }
}

void tst_QStateMachine::eventTransitions()
{
    QPushButton button;
    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());

        QMouseEventTransition *trans = new QMouseEventTransition(&button, QEvent::MouseButtonPress, Qt::LeftButton);
        QCOMPARE(trans->eventType(), QEvent::MouseButtonPress);
        QCOMPARE(trans->button(), Qt::LeftButton);
        trans->setTargetState(s1);
        s0->addTransition(trans);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        QTest::mousePress(&button, Qt::LeftButton);
        QCoreApplication::processEvents();

        QTRY_COMPARE(finishedSpy.count(), 1);
    }
    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());

        QEventTransition *trans = new QEventTransition();
        QCOMPARE(trans->eventSource(), (QObject*)0);
        QCOMPARE(trans->eventType(), QEvent::None);
        trans->setEventSource(&button);
        QCOMPARE(trans->eventSource(), (QObject*)&button);
        trans->setEventType(QEvent::MouseButtonPress);
        QCOMPARE(trans->eventType(), QEvent::MouseButtonPress);
        trans->setTargetState(s1);
        s0->addTransition(trans);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        QTest::mousePress(&button, Qt::LeftButton);
        QCoreApplication::processEvents();

        QTRY_COMPARE(finishedSpy.count(), 1);
    }
    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());

        QMouseEventTransition *trans = new QMouseEventTransition();
        QCOMPARE(trans->eventSource(), (QObject*)0);
        QCOMPARE(trans->eventType(), QEvent::None);
        QCOMPARE(trans->button(), Qt::NoButton);
        trans->setEventSource(&button);
        trans->setEventType(QEvent::MouseButtonPress);
        trans->setButton(Qt::LeftButton);
        trans->setTargetState(s1);
        s0->addTransition(trans);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        QTest::mousePress(&button, Qt::LeftButton);
        QCoreApplication::processEvents();

        QTRY_COMPARE(finishedSpy.count(), 1);
    }

    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());

        QKeyEventTransition *trans = new QKeyEventTransition(&button, QEvent::KeyPress, Qt::Key_A);
        QCOMPARE(trans->eventType(), QEvent::KeyPress);
        QCOMPARE(trans->key(), (int)Qt::Key_A);
        trans->setTargetState(s1);
        s0->addTransition(trans);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        QTest::keyPress(&button, Qt::Key_A);
        QCoreApplication::processEvents();

        QTRY_COMPARE(finishedSpy.count(), 1);
    }
    {
        QStateMachine machine;
        QState *s0 = new QState(machine.rootState());
        QFinalState *s1 = new QFinalState(machine.rootState());

        QKeyEventTransition *trans = new QKeyEventTransition();
        QCOMPARE(trans->eventSource(), (QObject*)0);
        QCOMPARE(trans->eventType(), QEvent::None);
        QCOMPARE(trans->key(), 0);
        trans->setEventSource(&button);
        trans->setEventType(QEvent::KeyPress);
        trans->setKey(Qt::Key_A);
        trans->setTargetState(s1);
        s0->addTransition(trans);

        QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
        machine.setInitialState(s0);
        machine.start();
        QCoreApplication::processEvents();

        QTest::keyPress(&button, Qt::Key_A);
        QCoreApplication::processEvents();

        QTRY_COMPARE(finishedSpy.count(), 1);
    }
}

void tst_QStateMachine::historyStates()
{
    QStateMachine machine;
    QState *root = machine.rootState();
      QState *s0 = new QState(root);
        QState *s00 = new QState(s0);
        QState *s01 = new QState(s0);
        QHistoryState *s0h = s0->addHistoryState();
      QState *s1 = new QState(root);
      QFinalState *s2 = new QFinalState(root);

    s00->addTransition(new StringTransition("a", s01));
    s0->addTransition(new StringTransition("b", s1));
    s1->addTransition(new StringTransition("c", s0h));
    s0->addTransition(new StringTransition("d", s2));

    root->setInitialState(s0);
    s0->setInitialState(s00);

    QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
    machine.start();
    QCoreApplication::processEvents();
    QCOMPARE(machine.configuration().size(), 2);
    QVERIFY(machine.configuration().contains(s0));
    QVERIFY(machine.configuration().contains(s00));

    machine.postEvent(new StringEvent("a"));
    QCoreApplication::processEvents();
    QCOMPARE(machine.configuration().size(), 2);
    QVERIFY(machine.configuration().contains(s0));
    QVERIFY(machine.configuration().contains(s01));

    machine.postEvent(new StringEvent("b"));
    QCoreApplication::processEvents();
    QCOMPARE(machine.configuration().size(), 1);
    QVERIFY(machine.configuration().contains(s1));

    machine.postEvent(new StringEvent("c"));
    QCoreApplication::processEvents();
    QCOMPARE(machine.configuration().size(), 2);
    QVERIFY(machine.configuration().contains(s0));
    QVERIFY(machine.configuration().contains(s01));

    machine.postEvent(new StringEvent("d"));
    QCoreApplication::processEvents();
    QCOMPARE(machine.configuration().size(), 1);
    QVERIFY(machine.configuration().contains(s2));

    QTRY_COMPARE(finishedSpy.count(), 1);
}

class TestStateAction : public QStateAction
{
public:
    TestStateAction() : m_didExecute(false)
        {}
    bool didExecute() const {
        return m_didExecute;
    }
protected:
    void execute() {
        m_didExecute = true;
    }
private:
    bool m_didExecute;
};

void tst_QStateMachine::stateActions()
{
    QStateMachine machine;
    QState *s1 = new QState(machine.rootState());

    QVERIFY(s1->entryActions().isEmpty());
    QVERIFY(s1->exitActions().isEmpty());

    QTest::ignoreMessage(QtWarningMsg, "QActionState::addEntryAction: cannot add null action");
    s1->addEntryAction(0);
    QTest::ignoreMessage(QtWarningMsg, "QActionState::addExitAction: cannot add null action");
    s1->addExitAction(0);
    QTest::ignoreMessage(QtWarningMsg, "QActionState::removeEntryAction: cannot remove null action");
    s1->removeEntryAction(0);
    QTest::ignoreMessage(QtWarningMsg, "QActionState::removeExitAction: cannot remove null action");
    s1->removeExitAction(0);

    QFinalState *s2 = new QFinalState(machine.rootState());
    s1->addTransition(s2);

    machine.setInitialState(s1);
    QSignalSpy finishedSpy(&machine, SIGNAL(finished()));

    QObject *obj = new QObject();
    QStateInvokeMethodAction *ima = new QStateInvokeMethodAction(obj, "deleteLater");
    QPointer<QObject> ptr(obj);
    QVERIFY(ptr != 0);
    s1->addEntryAction(ima);
    finishedSpy.clear();
    machine.start();
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCoreApplication::processEvents();
    QVERIFY(ptr == 0);

    s1->removeEntryAction(ima);

    s1->invokeMethodOnEntry(ima, "deleteLater");
    QCOMPARE(s1->entryActions().size(), 1);

    ptr = ima;
    QVERIFY(ptr != 0);
    finishedSpy.clear();
    machine.start();
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCoreApplication::processEvents();
    QVERIFY(ptr == 0);

    while (!s1->entryActions().isEmpty()) {
        QStateAction *act = s1->entryActions().first();
        s1->removeEntryAction(act);
        delete act;
    }

    TestStateAction *act1 = new TestStateAction();
    s1->addEntryAction(act1);
    TestStateAction *act2 = new TestStateAction();
    s1->addExitAction(act2);
    QVERIFY(!act1->didExecute());
    QVERIFY(!act2->didExecute());

    finishedSpy.clear();
    machine.start();
    QTRY_COMPARE(finishedSpy.count(), 1);

    QVERIFY(act1->didExecute());
    QVERIFY(act2->didExecute());

    QCOMPARE(s1->entryActions().size(), 1);
    QCOMPARE(s2->entryActions().size(), 0);
    s2->addEntryAction(act1); // should remove it from s1
    QCOMPARE(s1->entryActions().size(), 0);
    QCOMPARE(s2->entryActions().size(), 1);
    QCOMPARE(act1->parent(), (QObject*)s2);

    QCOMPARE(s2->exitActions().size(), 0);
    s2->addExitAction(act1); // should remove entry action
    QCOMPARE(s2->exitActions().size(), 1);
    QCOMPARE(s2->entryActions().size(), 0);
    QCOMPARE(act1->parent(), (QObject*)s2);
}

void tst_QStateMachine::transitionActions()
{
    QStateMachine machine;
    QState *s1 = new QState(machine.rootState());

    QFinalState *s2 = new QFinalState(machine.rootState());
    QTransition *trans = new EventTransition(QEvent::User, s2);
    s1->addTransition(trans);
    QVERIFY(trans->actions().isEmpty());
    QTest::ignoreMessage(QtWarningMsg, "QTransition::addAction: cannot add null action");
    trans->addAction(0);
    QVERIFY(trans->actions().isEmpty());

    TestStateAction *act = new TestStateAction();
    trans->addAction(act);
    QCOMPARE(trans->actions().size(), 1);
    QCOMPARE(trans->actions().at(0), (QStateAction*)act);
    QCOMPARE(act->parent(), (QObject*)trans);
    QVERIFY(!act->didExecute());

    trans->removeAction(act);
    QVERIFY(trans->actions().isEmpty());
    QCOMPARE(act->parent(), (QObject*)0);

    trans->addAction(act);

    QSignalSpy finishedSpy(&machine, SIGNAL(finished()));
    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();
    QTRY_COMPARE(finishedSpy.count(), 1);
    QVERIFY(act->didExecute());

    trans->invokeMethodOnTransition(act, "deleteLater");

    QPointer<QStateAction> ptr(act);
    QVERIFY(ptr != 0);
    finishedSpy.clear();
    machine.start();
    QCoreApplication::processEvents();

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCoreApplication::processEvents();
    QVERIFY(ptr == 0);
}

void tst_QStateMachine::defaultGlobalRestorePolicy()
{
    QStateMachine machine;    

    QObject *propertyHolder = new QObject();
    propertyHolder->setProperty("a", 1);
    propertyHolder->setProperty("b", 2);

    QState *s1 = new QState(machine.rootState());
    s1->setPropertyOnEntry(propertyHolder, "a", 3);

    QState *s2 = new QState(machine.rootState());
    s2->setPropertyOnEntry(propertyHolder, "b", 4);

    QState *s3 = new QState(machine.rootState());

    s1->addTransition(new EventTransition(QEvent::User, s2));
    s2->addTransition(new EventTransition(QEvent::User, s3));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();
    
    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 2);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 4);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 4);    
}

void tst_QStateMachine::restorePolicyNotInherited()
{
    QStateMachine machine;    

    QObject *propertyHolder = new QObject();
    propertyHolder->setProperty("a", 1);
    propertyHolder->setProperty("b", 2);

    QState *parentState = new QState(machine.rootState());
    parentState->setObjectName("parentState");
    parentState->setRestorePolicy(QState::RestoreProperties);

    QState *s1 = new QState(parentState);
    s1->setObjectName("s1");
    s1->setPropertyOnEntry(propertyHolder, "a", 3);
    parentState->setInitialState(s1);

    QState *s2 = new QState(parentState);
    s2->setObjectName("s2");
    s2->setPropertyOnEntry(propertyHolder, "b", 4);

    QState *s3 = new QState(parentState);
    s3->setObjectName("s3");

    s1->addTransition(new EventTransition(QEvent::User, s2));
    s2->addTransition(new EventTransition(QEvent::User, s3));

    machine.setInitialState(parentState);
    machine.start();
    QCoreApplication::processEvents();
    
    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 2);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 4);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 4);    

}

void tst_QStateMachine::globalRestorePolicySetToDoNotRestore()
{
    QStateMachine machine;
    machine.setGlobalRestorePolicy(QState::DoNotRestoreProperties);

    QObject *propertyHolder = new QObject();
    propertyHolder->setProperty("a", 1);
    propertyHolder->setProperty("b", 2);

    QState *s1 = new QState(machine.rootState());
    s1->setPropertyOnEntry(propertyHolder, "a", 3);

    QState *s2 = new QState(machine.rootState());
    s2->setPropertyOnEntry(propertyHolder, "b", 4);

    QState *s3 = new QState(machine.rootState());

    s1->addTransition(new EventTransition(QEvent::User, s2));
    s2->addTransition(new EventTransition(QEvent::User, s3));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();
    
    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 2);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 4);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 4);       
}

void tst_QStateMachine::setRestorePolicyToDoNotRestore()
{
    QObject *object = new QObject();
    object->setProperty("a", 1);
    object->setProperty("b", 2);

    QStateMachine machine;

    QState *S1 = new QState();
    S1->setObjectName("S1");
    S1->setPropertyOnEntry(object, "a", 3);
    S1->setRestorePolicy(QState::DoNotRestoreProperties);
    machine.addState(S1);

    QState *S2 = new QState();
    S2->setObjectName("S2");
    S2->setPropertyOnEntry(object, "b", 5);
    S2->setRestorePolicy(QState::DoNotRestoreProperties);
    machine.addState(S2);

    QState *S3 = new QState();
    S3->setObjectName("S3");
    S3->setRestorePolicy(QState::DoNotRestoreProperties);
    machine.addState(S3);

    QFinalState *S4 = new QFinalState();
    machine.addState(S4);

    S1->addTransition(new EventTransition(QEvent::User, S2));
    S2->addTransition(new EventTransition(QEvent::User, S3));
    S3->addTransition(S4);

    machine.setInitialState(S1);
    machine.start();
    QCoreApplication::processEvents();

    QCOMPARE(object->property("a").toInt(), 3);
    QCOMPARE(object->property("b").toInt(), 2);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(object->property("a").toInt(), 3);
    QCOMPARE(object->property("b").toInt(), 5);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(object->property("a").toInt(), 3);
    QCOMPARE(object->property("b").toInt(), 5);
}

void tst_QStateMachine::setGlobalRestorePolicyToGlobalRestore()
{
    s_countWarnings = false;
    QStateMachine machine;
    machine.setGlobalRestorePolicy(QState::GlobalRestorePolicy);

    QCOMPARE(machine.globalRestorePolicy(), QState::DoNotRestoreProperties);
    QCOMPARE(s_msgType, QtWarningMsg);

    s_msgType = QtDebugMsg;
    machine.setGlobalRestorePolicy(QState::RestoreProperties);
    machine.setGlobalRestorePolicy(QState::GlobalRestorePolicy);

    QCOMPARE(machine.globalRestorePolicy(), QState::RestoreProperties);
    QCOMPARE(s_msgType, QtWarningMsg);
}

void tst_QStateMachine::restorePolicyOnChildState()
{
    QStateMachine machine;

    QObject *propertyHolder = new QObject();
    propertyHolder->setProperty("a", 1);
    propertyHolder->setProperty("b", 2);

    QState *parentState = new QState(machine.rootState());
    parentState->setObjectName("parentState");

    QState *s1 = new QState(parentState);
    s1->setRestorePolicy(QState::RestoreProperties);
    s1->setObjectName("s1");
    s1->setPropertyOnEntry(propertyHolder, "a", 3);
    parentState->setInitialState(s1);

    QState *s2 = new QState(parentState);
    s2->setRestorePolicy(QState::RestoreProperties);
    s2->setObjectName("s2");
    s2->setPropertyOnEntry(propertyHolder, "b", 4);

    QState *s3 = new QState(parentState);
    s3->setRestorePolicy(QState::RestoreProperties);
    s3->setObjectName("s3");

    s1->addTransition(new EventTransition(QEvent::User, s2));
    s2->addTransition(new EventTransition(QEvent::User, s3));

    machine.setInitialState(parentState);
    machine.start();
    QCoreApplication::processEvents();
    
    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 2);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 1);
    QCOMPARE(propertyHolder->property("b").toInt(), 4);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 1);
    QCOMPARE(propertyHolder->property("b").toInt(), 2);    
}

void tst_QStateMachine::globalRestorePolicySetToRestore()
{
    QStateMachine machine;    
    machine.setGlobalRestorePolicy(QState::RestoreProperties);

    QObject *propertyHolder = new QObject();
    propertyHolder->setProperty("a", 1);
    propertyHolder->setProperty("b", 2);

    QState *s1 = new QState(machine.rootState());
    s1->setPropertyOnEntry(propertyHolder, "a", 3);

    QState *s2 = new QState(machine.rootState());
    s2->setPropertyOnEntry(propertyHolder, "b", 4);

    QState *s3 = new QState(machine.rootState());

    s1->addTransition(new EventTransition(QEvent::User, s2));
    s2->addTransition(new EventTransition(QEvent::User, s3));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();
    
    QCOMPARE(propertyHolder->property("a").toInt(), 3);
    QCOMPARE(propertyHolder->property("b").toInt(), 2);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 1);
    QCOMPARE(propertyHolder->property("b").toInt(), 4);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    QCOMPARE(propertyHolder->property("a").toInt(), 1);
    QCOMPARE(propertyHolder->property("b").toInt(), 2);    
}

void tst_QStateMachine::mixedRestoreProperties()
{
    QStateMachine machine;    

    QObject *propertyHolder = new QObject();
    propertyHolder->setProperty("a", 1);    

    QState *s1 = new QState(machine.rootState());
    s1->setRestorePolicy(QState::RestoreProperties);
    s1->setPropertyOnEntry(propertyHolder, "a", 3);

    QState *s2 = new QState(machine.rootState());
    s2->setPropertyOnEntry(propertyHolder, "a", 4);

    QState *s3 = new QState(machine.rootState());

    QState *s4 = new QState(machine.rootState());
    s4->setPropertyOnEntry(propertyHolder, "a", 5);

    QState *s5 = new QState(machine.rootState());
    s5->setRestorePolicy(QState::RestoreProperties);
    s5->setPropertyOnEntry(propertyHolder, "a", 6);

    s1->addTransition(new EventTransition(QEvent::User, s2));
    s2->addTransition(new EventTransition(QEvent::User, s3));
    s3->addTransition(new EventTransition(QEvent::User, s4));
    s4->addTransition(new EventTransition(QEvent::User, s5));
    s5->addTransition(new EventTransition(QEvent::User, s3));

    machine.setInitialState(s1);
    machine.start();
    QCoreApplication::processEvents();
    
    // Enter s1, save current
    QCOMPARE(propertyHolder->property("a").toInt(), 3);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    // Enter s2, restorePolicy == DoNotRestore, so restore all properties
    QCOMPARE(propertyHolder->property("a").toInt(), 4);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    // Enter s3
    QCOMPARE(propertyHolder->property("a").toInt(), 4);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    // Enter s4
    QCOMPARE(propertyHolder->property("a").toInt(), 5);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    // Enter s5, save current
    QCOMPARE(propertyHolder->property("a").toInt(), 6);

    machine.postEvent(new QEvent(QEvent::User));
    QCoreApplication::processEvents();

    // Enter s3, restore
    QCOMPARE(propertyHolder->property("a").toInt(), 5);
}

void tst_QStateMachine::transitionWithParent()
{
    QStateMachine machine;
    QState *s1 = new QState(machine.rootState());
    QState *s2 = new QState(machine.rootState());
    EventTransition *trans = new EventTransition(QEvent::User, s2, s1);
    QCOMPARE(trans->sourceState(), s1);
    QCOMPARE(trans->targetState(), (QAbstractState*)s2);
    QCOMPARE(trans->targetStates().size(), 1);
    QCOMPARE(trans->targetStates().at(0), (QAbstractState*)s2);
}

QTEST_MAIN(tst_QStateMachine)
#include "tst_qstatemachine.moc"
