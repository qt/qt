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
#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicsrectangle_p.h>
#include <private/qmlgraphicsimage_p.h>
#include <private/qmlgraphicsanimatedimage_p.h>

#include "../shared/testhttpserver.h"

#define TRY_WAIT(expr) \
    do { \
        for (int ii = 0; ii < 6; ++ii) { \
            if ((expr)) break; \
            QTest::qWait(50); \
        } \
        QVERIFY((expr)); \
    } while (false)


class tst_animatedimage : public QObject
{
    Q_OBJECT
public:
    tst_animatedimage() {}

private slots:
    void play();
    void pause();
    void stopped();
    void setFrame();
    void frameCount();
    void remote();
    void remote_data();
    void invalidSource();
};

void tst_animatedimage::play()
{
    QmlEngine engine;
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickman.qml"));
    QmlGraphicsAnimatedImage *anim = qobject_cast<QmlGraphicsAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(anim->isPlaying());

    delete anim;
}

void tst_animatedimage::pause()
{
    QmlEngine engine;
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickmanpause.qml"));
    QmlGraphicsAnimatedImage *anim = qobject_cast<QmlGraphicsAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(anim->isPlaying());
    QVERIFY(anim->isPaused());

    delete anim;
}

void tst_animatedimage::stopped()
{
    QmlEngine engine;
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickmanstopped.qml"));
    QmlGraphicsAnimatedImage *anim = qobject_cast<QmlGraphicsAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(!anim->isPlaying());
    QCOMPARE(anim->currentFrame(), 0);

    delete anim;
}

void tst_animatedimage::setFrame()
{
    QmlEngine engine;
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickmanpause.qml"));
    QmlGraphicsAnimatedImage *anim = qobject_cast<QmlGraphicsAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(anim->isPlaying());
    QCOMPARE(anim->currentFrame(), 2);

    delete anim;
}

void tst_animatedimage::frameCount()
{
    QmlEngine engine;
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/colors.qml"));
    QmlGraphicsAnimatedImage *anim = qobject_cast<QmlGraphicsAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(anim->isPlaying());
    QCOMPARE(anim->frameCount(), 0); // GIF doesn't support frameCount until first pass through
    QTest::qWait(600 + 100);
    QCOMPARE(anim->frameCount(), 3);

    delete anim;
}

void tst_animatedimage::remote()
{
    QFETCH(QString, fileName);
    QFETCH(bool, paused);

    TestHTTPServer server(14445);
    QVERIFY(server.isValid());
    server.serveDirectory(SRCDIR "/data");

    QmlEngine engine;
    QmlComponent component(&engine, QUrl("http://127.0.0.1:14445/" + fileName));
    TRY_WAIT(component.isReady());

    QmlGraphicsAnimatedImage *anim = qobject_cast<QmlGraphicsAnimatedImage *>(component.create());
    QVERIFY(anim);

    TRY_WAIT(anim->isPlaying());
    if (paused) {
        TRY_WAIT(anim->isPaused());
        QCOMPARE(anim->currentFrame(), 2);
    }

    delete anim;
}

void tst_animatedimage::remote_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("paused");

    QTest::newRow("playing") << "stickman.qml" << false;
    QTest::newRow("paused") << "stickmanpause.qml" << true;
}

void tst_animatedimage::invalidSource()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData("import Qt 4.6\n AnimatedImage { source: \"no-such-file.gif\" }", QUrl::fromLocalFile(""));
    QVERIFY(component.isReady());

    QTest::ignoreMessage(QtWarningMsg, "Error Reading Animated Image File  QUrl( \"file:no-such-file.gif\" )  ");

    QmlGraphicsAnimatedImage *anim = qobject_cast<QmlGraphicsAnimatedImage *>(component.create());
    QVERIFY(anim);

    QVERIFY(!anim->isPlaying());
    QVERIFY(!anim->isPaused());
    QCOMPARE(anim->currentFrame(), 0);
    QCOMPARE(anim->frameCount(), 0);
}

QTEST_MAIN(tst_animatedimage)

#include "tst_animatedimage.moc"
