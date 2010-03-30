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



#include <QtTest/QtTest>
#include <QtCore/qlocale.h>
#include <qaudiooutput.h>
#include <qaudiodeviceinfo.h>
#include <qaudio.h>
#include <private/qsoundeffect_p.h>


class tst_QSoundEffect : public QObject
{
    Q_OBJECT
public:
    tst_QSoundEffect(QObject* parent=0) : QObject(parent) {}

private slots:
    void initTestCase();
    void testSource();
    void testLooping();
    void testVolume();
    void testMuting();

private:
    QSoundEffect* sound;
};

void tst_QSoundEffect::initTestCase()
{
#ifndef QT_MULTIMEDIA_QMEDIAPLAYER
    sound = new QSoundEffect;

    QVERIFY(sound->source().isEmpty());
    QVERIFY(sound->loops() == 1);
    QVERIFY(sound->volume() == 100);
    QVERIFY(sound->isMuted() == false);
#endif
}

void tst_QSoundEffect::testSource()
{
#ifndef QT_MULTIMEDIA_QMEDIAPLAYER
    QSignalSpy readSignal(sound, SIGNAL(sourceChanged()));

    QUrl url = QUrl::fromLocalFile(QString("%1%2").arg(SRCDIR).arg("test.wav"));
    sound->setSource(url);

    QCOMPARE(sound->source(),url);
    QCOMPARE(readSignal.count(),1);

    QTestEventLoop::instance().enterLoop(1);
    sound->play();

    QTest::qWait(3000);
#endif
}

void tst_QSoundEffect::testLooping()
{
#ifndef QT_MULTIMEDIA_QMEDIAPLAYER
    QSignalSpy readSignal(sound, SIGNAL(loopsChanged()));

    sound->setLoops(5);
    QCOMPARE(sound->loops(),5);

    sound->play();

    // test.wav is about 200ms, wait until it has finished playing 5 times
    QTest::qWait(3000);
#endif
}

void tst_QSoundEffect::testVolume()
{
#ifndef QT_MULTIMEDIA_QMEDIAPLAYER
    QSignalSpy readSignal(sound, SIGNAL(volumeChanged()));

    sound->setVolume(50);
    QCOMPARE(sound->volume(),50);

    QTest::qWait(20);
    QCOMPARE(readSignal.count(),1);
#endif
}

void tst_QSoundEffect::testMuting()
{
#ifndef QT_MULTIMEDIA_QMEDIAPLAYER
    QSignalSpy readSignal(sound, SIGNAL(mutedChanged()));

    sound->setMuted(true);
    QCOMPARE(sound->isMuted(),true);

    QTest::qWait(20);
    QCOMPARE(readSignal.count(),1);

    delete sound;
#endif
}

QTEST_MAIN(tst_QSoundEffect)

#include "tst_qsoundeffect.moc"
