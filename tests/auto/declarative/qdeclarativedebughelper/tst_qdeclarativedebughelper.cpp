/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qtest.h>

#include <QDeclarativeEngine>
#include <private/qdeclarativeengine_p.h>
#include <QAbstractAnimation>
#include <private/qabstractanimation_p.h>

// We have copied the header which is used in the qmljsdebugger (part of QtCreator)
// to catch BC changes. Don't update it unless you know what you are doing!
#include "private_headers/qdeclarativedebughelper_p.h"

class tst_qdeclarativedebughelper : public QObject {
    Q_OBJECT
private slots:
    void getScriptEngine();
    void setAnimationSlowDownFactor();
};

class TestAnimation : public QAbstractAnimation {
public:
    int updateCalled;

    TestAnimation() : updateCalled(0) {}

    virtual void updateCurrentTime(int /*currentTime*/) {
        updateCalled++;
    }
    virtual int duration() const {
        return 100;
    }
};

void tst_qdeclarativedebughelper::getScriptEngine()
{
    QDeclarativeEngine engine;

    QScriptEngine *scriptEngine = QDeclarativeDebugHelper::getScriptEngine(&engine);
    QVERIFY(scriptEngine);
    QCOMPARE(scriptEngine, QDeclarativeEnginePrivate::getScriptEngine(&engine));
}

void tst_qdeclarativedebughelper::setAnimationSlowDownFactor()
{
    TestAnimation animation;

    // first check whether setup works
    QCOMPARE(animation.updateCalled, 0);
    animation.start();
    QTest::qWait(animation.totalDuration() + 50);
#ifdef Q_OS_WIN
    if (animation.state() != QAbstractAnimation::Stopped)
        QEXPECT_FAIL("", "On windows, consistent timing is not working properly due to bad timer resolution", Abort);
#endif
    QCOMPARE(animation.state(), QAbstractAnimation::Stopped);
    QVERIFY(animation.updateCalled > 1);

    // check if we can pause all animations
    animation.updateCalled = 0;
    QDeclarativeDebugHelper::setAnimationSlowDownFactor(0.0);
    animation.start();
    QTest::qWait(animation.totalDuration() + 50);
    QVERIFY(animation.updateCalled <= 1); // updateCurrentTime seems to be called at  least once

    // now run them again
    animation.updateCalled = 0;
    QDeclarativeDebugHelper::setAnimationSlowDownFactor(2.0);
    animation.start();
    QTest::qWait(animation.totalDuration() + 50);
    QVERIFY(animation.updateCalled > 1);
}

QTEST_MAIN(tst_qdeclarativedebughelper)

#include "tst_qdeclarativedebughelper.moc"

