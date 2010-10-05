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
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <private/qdeclarativerectangle_p.h>
#include <private/qdeclarativeimage_p.h>
#include <private/qdeclarativeanimatedimage_p.h>

#include "../shared/testhttpserver.h"
#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativeanimatedimage : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeanimatedimage() {}

private slots:
    void play();
    void pause();
    void stopped();
    void setFrame();
    void frameCount();
    void remote();
    void remote_data();
    void sourceSize();
    void sourceSizeReadOnly();
    void invalidSource();
};

void tst_qdeclarativeanimatedimage::play()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickman.qml"));
    QDeclarativeAnimatedImage *anim = qobject_cast<QDeclarativeAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(anim->isPlaying());

    delete anim;
}

void tst_qdeclarativeanimatedimage::pause()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickmanpause.qml"));
    QDeclarativeAnimatedImage *anim = qobject_cast<QDeclarativeAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(anim->isPlaying());
    QVERIFY(anim->isPaused());

    delete anim;
}

void tst_qdeclarativeanimatedimage::stopped()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickmanstopped.qml"));
    QDeclarativeAnimatedImage *anim = qobject_cast<QDeclarativeAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(!anim->isPlaying());
    QCOMPARE(anim->currentFrame(), 0);

    delete anim;
}

void tst_qdeclarativeanimatedimage::setFrame()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickmanpause.qml"));
    QDeclarativeAnimatedImage *anim = qobject_cast<QDeclarativeAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(anim->isPlaying());
    QCOMPARE(anim->currentFrame(), 2);

    delete anim;
}

void tst_qdeclarativeanimatedimage::frameCount()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/colors.qml"));
    QDeclarativeAnimatedImage *anim = qobject_cast<QDeclarativeAnimatedImage *>(component.create());
    QVERIFY(anim);
    QVERIFY(anim->isPlaying());
    QCOMPARE(anim->frameCount(), 3);

    delete anim;
}

void tst_qdeclarativeanimatedimage::remote()
{
    QFETCH(QString, fileName);
    QFETCH(bool, paused);

    TestHTTPServer server(14449);
    QVERIFY(server.isValid());
    server.serveDirectory(SRCDIR "/data");

    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl("http://127.0.0.1:14449/" + fileName));
    QTRY_VERIFY(component.isReady());

    QDeclarativeAnimatedImage *anim = qobject_cast<QDeclarativeAnimatedImage *>(component.create());
    QVERIFY(anim);

    QTRY_VERIFY(anim->isPlaying());
    if (paused) {
        QTRY_VERIFY(anim->isPaused());
        QCOMPARE(anim->currentFrame(), 2);
    }
    QVERIFY(anim->status() != QDeclarativeAnimatedImage::Error);

    delete anim;
}

void tst_qdeclarativeanimatedimage::sourceSize()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickmanscaled.qml"));
    QDeclarativeAnimatedImage *anim = qobject_cast<QDeclarativeAnimatedImage *>(component.create());
    QVERIFY(anim);
    QCOMPARE(anim->width(),240.0);
    QCOMPARE(anim->height(),180.0);
    QCOMPARE(anim->sourceSize(),QSize(160,120));

    delete anim;
}

void tst_qdeclarativeanimatedimage::sourceSizeReadOnly()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/stickmanerror1.qml"));
    QVERIFY(component.isError());
    QCOMPARE(component.errors().at(0).description(), QString("Invalid property assignment: \"sourceSize\" is a read-only property"));
}

void tst_qdeclarativeanimatedimage::remote_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("paused");

    QTest::newRow("playing") << "stickman.qml" << false;
    QTest::newRow("paused") << "stickmanpause.qml" << true;
}

void tst_qdeclarativeanimatedimage::invalidSource()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 1.0\n AnimatedImage { source: \"no-such-file.gif\" }", QUrl::fromLocalFile(""));
    QVERIFY(component.isReady());

    QTest::ignoreMessage(QtWarningMsg, "file::2:2: QML AnimatedImage: Error Reading Animated Image File file:no-such-file.gif");

    QDeclarativeAnimatedImage *anim = qobject_cast<QDeclarativeAnimatedImage *>(component.create());
    QVERIFY(anim);

    QVERIFY(!anim->isPlaying());
    QVERIFY(!anim->isPaused());
    QCOMPARE(anim->currentFrame(), 0);
    QCOMPARE(anim->frameCount(), 0);
}

QTEST_MAIN(tst_qdeclarativeanimatedimage)

#include "tst_qdeclarativeanimatedimage.moc"
