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
/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QtTest/QtTest>
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QUrl>

#ifndef QT_NO_PHONON
#include <phonon/path.h>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/phononnamespace.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/videowidget.h>
#include <phonon/backendcapabilities.h>

#include "qtesthelper.h"
#include <cstdlib>
#endif

#ifndef Q_WS_WIN
#include <unistd.h>
#endif

#ifdef Q_OS_WINCE
#define MEDIA_FILE "/sax.wav"
#define MEDIA_FILEPATH ":/media/sax.wav"
const qint64 SEEK_BACKWARDS = 2000;
const qint64 ALLOWED_TIME_FOR_SEEKING = 1500; // 1.5s
const qint64 SEEKING_TOLERANCE = 250;
#else 
#if defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(Q_OS_SYMBIAN)
#define MEDIA_FILE "/sax.mp3"
#define MEDIA_FILEPATH ":/media/sax.mp3"
#else
#define MEDIA_FILE "/sax.ogg"
#define MEDIA_FILEPATH ":/media/sax.ogg"
#endif
const qint64 SEEK_BACKWARDS = 4000;
const qint64 ALLOWED_TIME_FOR_SEEKING = 1000; // 1s
const qint64 SEEKING_TOLERANCE = 0;
#endif //Q_OS_WINCE

class tst_MediaObject : public QObject
{
    Q_OBJECT
    public:
        tst_MediaObject()
            : m_success(false)
    {
        qputenv("PHONON_GST_AUDIOSINK", "fake");
    }

#ifndef QT_NO_PHONON

    Q_SIGNALS:
        void continueTestPlayOnFinish();

    protected Q_SLOTS:
        void enqueueMedia();
        void setMediaAndPlay();
        void stateChanged(Phonon::State, Phonon::State);
    private Q_SLOTS:
        void init();
        void cleanup();

        void testPlayFromResource();
        void testPlayIllegalFile();
        void initTestCase();
        void checkForDefaults();

        // state change tests
        void stopToStop();
        void stopToPause();
        void stopToPlay();
        void playToPlay();
        void playToPause();
        void playToStop();
        void pauseToPause();
        void pauseToPlay();
        void pauseToStop();
        void playSDP();

        void testPrefinishMark();
        void testSeek();
        void testTickSignal();
        void testJustInTimeQueuing();
        void testPlayOnFinish();
        void testPlayBeforeFinish();
        void testPauseOnFinish();
        void testReconnectBetweenTwoMediaObjects();
        void volumeSliderMuteVisibility();
        void cleanupTestCase();
    private:
        void _startPlayback(Phonon::State currentState = Phonon::StoppedState);
        void _stopPlayback(Phonon::State currentState);
        void _pausePlayback();
        void _testOneSeek(qint64 seekTo);

        QUrl m_url;
        Phonon::MediaObject *m_media;
        QSignalSpy *m_stateChangedSignalSpy;
        QString m_tmpFileName;

        static void copyMediaFile(const QString &original,
                                  const QString &name,
                                  QString &resultFilePath,
                                  QUrl *const asURL = 0);
#endif //QT_NO_PHONON
        bool m_success;
};

#ifndef QT_NO_PHONON

#define startPlayback() _startPlayback(); if (!m_success) return; m_success = false;
#define startPlayback2(currentState) _startPlayback(currentState); if (!m_success) return; m_success = false;
#define stopPlayback(currentState) _stopPlayback(currentState); if (!m_success) return; m_success = false;
#define pausePlayback() _pausePlayback(); if (!m_success) return; m_success = false;
#define testOneSeek(seekTo) _testOneSeek(seekTo); if (!m_success) return; m_success = false;

const qint64 ALLOWED_SEEK_INACCURACY = 300; // 0.3s
const qint64 ALLOWED_TICK_INACCURACY = 350; // allow +/- 350 ms inaccuracy

using namespace Phonon;

static qint64 castQVariantToInt64(const QVariant &variant)
{
    return *reinterpret_cast<const qint64 *>(variant.constData());
}

static qint32 castQVariantToInt32(const QVariant &variant)
{
    return *reinterpret_cast<const qint32 *>(variant.constData());
}

static const char *const red    = "\033[0;31m";
static const char *const green  = "\033[0;32m";
static const char *const yellow = "\033[0;33m";
static const char *const blue   = "\033[0;34m";
static const char *const purple = "\033[0;35m";
static const char *const cyan   = "\033[0;36m";
static const char *const white  = "\033[0;37m";
static const char *const normal = "\033[0m";

void tst_MediaObject::stateChanged(Phonon::State newstate, Phonon::State oldstate)
{
    if (newstate == Phonon::ErrorState) {
        QWARN(QByteArray(QByteArray(red) + ".......................................................... ") + QByteArray(QTest::toString(oldstate)) + " to " + QByteArray(QTest::toString(newstate)) + normal);
    } else {
        //qDebug() << ".........................................................." << cyan << QTest::toString(oldstate) << "to" << QTest::toString(newstate) << normal;
    }
}

void tst_MediaObject::testPlayFromResource()
{
#ifdef Q_OS_SYMBIAN
    QSKIP("Not implemented yet.", SkipAll);
    return;
#endif

    QFile file(MEDIA_FILEPATH);
    MediaObject media;
    media.setCurrentSource(&file);
    QVERIFY(media.state() != Phonon::ErrorState);
    if (media.state() != Phonon::StoppedState)
        QTest::waitForSignal(&media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 10000);
    QCOMPARE(media.state(), Phonon::StoppedState);
    media.play();
    if (media.state() != Phonon::PlayingState)
        QTest::waitForSignal(&media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 10000);
    QCOMPARE(media.state(), Phonon::PlayingState);
}

void tst_MediaObject::testPlayIllegalFile()
{
    QString filename = QDir::tempPath() + QString("/test.wav");
    QFile::remove(filename);
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    for (int i=0;i<0xffff;i++) {
        int r = qrand();
        file.write((const char*)&r, 2);
    }
    file.close();
    MediaObject media;
    media.setCurrentSource(filename);
    QTest::waitForSignal(&media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 10000);
    QCOMPARE(media.state(), Phonon::ErrorState);
    media.play();
    QCOMPARE(media.state(), Phonon::ErrorState);
    QFile::remove(filename);
}

void tst_MediaObject::init()
{
    QCOMPARE(m_media->outputPaths().size(), 1);
    if (m_media->state() == Phonon::ErrorState) {
        m_media->setCurrentSource(m_url);
        if (m_media->state() == Phonon::ErrorState) {
            QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)));
        }
        if (m_media->state() == Phonon::LoadingState) {
            QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)));
        }
        m_stateChangedSignalSpy->clear();
    }
}

void tst_MediaObject::cleanup()
{
    switch (m_media->state()) {
    case Phonon::PlayingState:
    case Phonon::BufferingState:
    case Phonon::PausedState:
        stopPlayback(m_media->state());
        break;
    default:
        break;
    }
    m_stateChangedSignalSpy->clear();
}

void tst_MediaObject::_startPlayback(Phonon::State currentState)
{
    m_stateChangedSignalSpy->clear();
    Phonon::State s = m_media->state();
    QCOMPARE(s, currentState);
    m_media->play();
    while (s != Phonon::PlayingState) {
        if (m_stateChangedSignalSpy->isEmpty()) {
            QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 3000);
            QApplication::processEvents();
        }
        while (!m_stateChangedSignalSpy->isEmpty()) {
            QList<QVariant> args = m_stateChangedSignalSpy->takeFirst();
            Phonon::State laststate = qvariant_cast<Phonon::State>(args.at(1));
            QCOMPARE(laststate, s);
            s = qvariant_cast<Phonon::State>(args.at(0));
            QVERIFY(s == Phonon::BufferingState || s == Phonon::PlayingState);
        }
    }
    QCOMPARE(s, Phonon::PlayingState);
    QCOMPARE(m_media->state(), Phonon::PlayingState);
    m_success = true;
}

void tst_MediaObject::_stopPlayback(Phonon::State currentState)
{
    QVERIFY(currentState != Phonon::ErrorState);
    m_stateChangedSignalSpy->clear();
    Phonon::State s = m_media->state();
    QCOMPARE(s, currentState);
    m_media->stop();
    while (s != Phonon::StoppedState) {
        if (m_stateChangedSignalSpy->isEmpty()) {
            QVERIFY(QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 4000));
        }
        while (!m_stateChangedSignalSpy->isEmpty()) {
            QList<QVariant> args = m_stateChangedSignalSpy->takeFirst();
            Phonon::State laststate = qvariant_cast<Phonon::State>(args.at(1));
            QCOMPARE(laststate, s);
            s = qvariant_cast<Phonon::State>(args.at(0));
            if (s == Phonon::StoppedState) {
                QVERIFY(m_stateChangedSignalSpy->isEmpty());
                break;
            }
            QVERIFY(s == Phonon::BufferingState || s == Phonon::PlayingState);
        }
    }
    QCOMPARE(s, Phonon::StoppedState);
    QCOMPARE(m_media->state(), Phonon::StoppedState);
    m_success = true;
}

void tst_MediaObject::_pausePlayback()
{
    m_stateChangedSignalSpy->clear();
    Phonon::State s = m_media->state();
    m_media->pause();
    while (s != Phonon::PausedState) {
        if (m_stateChangedSignalSpy->isEmpty()) {
            QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)));
        }
        while (!m_stateChangedSignalSpy->isEmpty()) {
            QList<QVariant> args = m_stateChangedSignalSpy->takeFirst();
            Phonon::State laststate = qvariant_cast<Phonon::State>(args.at(1));
            QCOMPARE(laststate, s);
            s = qvariant_cast<Phonon::State>(args.at(0));
            if (s == Phonon::PausedState) {
                QVERIFY(m_stateChangedSignalSpy->isEmpty());
                break;
            }
            QVERIFY(s == Phonon::BufferingState || s == Phonon::PlayingState);
        }
    }
    QCOMPARE(s, Phonon::PausedState);
    QCOMPARE(m_media->state(), Phonon::PausedState);
    m_success = true;
}

/*!
  Copies the file \a name to the testing area. The resulting file name path is
  returned in resultFilePath, and also set as a URL in \a asURL.
 */
void tst_MediaObject::copyMediaFile(const QString &original,
                                    const QString &name,
                                    QString &resultFilePath,
                                    QUrl *const asURL)
{
    resultFilePath = QDir::toNativeSeparators(QDir::tempPath() + name);
    if (asURL)
        *asURL = QUrl::fromLocalFile(resultFilePath);

    QFile::remove(resultFilePath);
    QVERIFY(QFile::copy(original, resultFilePath));
    QFile::setPermissions(resultFilePath, QFile::permissions(resultFilePath) | QFile::WriteOther);
}

void tst_MediaObject::initTestCase()
{
    QCoreApplication::setApplicationName("tst_MediaObject");
    m_stateChangedSignalSpy = 0;
    m_media = 0;

#ifdef Q_OS_WINCE
    QString pluginsPath = QLibraryInfo::location(QLibraryInfo::PluginsPath);
#ifdef DEBUG
    QVERIFY(QFile::exists(pluginsPath + "/phonon_backend/phonon_waveoutd4.dll") || QFile::exists(pluginsPath + "/phonon_backend/phonon_phonon_ds9d4.dll"));
#else
    QVERIFY(QFile::exists(pluginsPath + "/phonon_backend/phonon_waveout4.dll") || QFile::exists(pluginsPath + "/phonon_backend/phonon_phonon_ds94.dll"));
#endif
#endif


    m_url = qgetenv("PHONON_TESTURL");
    m_media = new MediaObject(this);
    connect(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), SLOT(stateChanged(Phonon::State, Phonon::State)));
    m_stateChangedSignalSpy = new QSignalSpy(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)));
    QVERIFY(m_stateChangedSignalSpy->isValid());
    m_stateChangedSignalSpy->clear();

    if (m_url.isEmpty())
        copyMediaFile(MEDIA_FILEPATH, MEDIA_FILE, m_tmpFileName, &m_url);
    
    qDebug() << "Using url:" << m_url.toString();

    // AudioOutput is needed else the backend might have no time source
    AudioOutput *audioOutput = new AudioOutput(Phonon::MusicCategory, this);
    //audioOutput->setVolume(0.0f);

    QSignalSpy totalTimeChangedSignalSpy(m_media, SIGNAL(totalTimeChanged(qint64)));
    QVERIFY(m_media->queue().isEmpty());
    QCOMPARE(m_media->currentSource().type(), MediaSource::Empty);
    QCOMPARE(m_media->state(), Phonon::LoadingState);
    QCOMPARE(m_stateChangedSignalSpy->count(), 0);

    m_media->setCurrentSource(m_url);
    QCOMPARE(m_media->currentSource().type(), MediaSource::Url);
    QCOMPARE(m_media->currentSource().url(), m_url);

    int emits = m_stateChangedSignalSpy->count();
    Phonon::State s = m_media->state();
    if (s == Phonon::LoadingState) {
        // still in LoadingState, there should be no state change
        QCOMPARE(emits, 0);
        QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 6000);
        emits = m_stateChangedSignalSpy->count();
        s = m_media->state();
    }
    if (s != Phonon::LoadingState) {
        // there should exactly be one state change
        QCOMPARE(emits, 1);
        QList<QVariant> args = m_stateChangedSignalSpy->takeFirst();
        Phonon::State newstate = qvariant_cast<Phonon::State>(args.at(0));
        Phonon::State oldstate = qvariant_cast<Phonon::State>(args.at(1));

        QCOMPARE(Phonon::LoadingState, oldstate);
        QCOMPARE(s, newstate);
        if (Phonon::ErrorState == s) {
#ifdef Q_WS_WIN
            if (m_media->errorString().contains(QLatin1String("no audio hardware is available")))
                QSKIP("On Windows we need an audio devide to perform the MediaObject tests", SkipAll); 
            else
#endif
            QFAIL("Loading the URL put the MediaObject into the ErrorState. Check that PHONON_TESTURL is set to a valid URL.");
        }
        QCOMPARE(Phonon::StoppedState, s);
        QCOMPARE(m_stateChangedSignalSpy->count(), 0);

        // check for totalTimeChanged signal
        QVERIFY(totalTimeChangedSignalSpy.count() > 0);
        args = totalTimeChangedSignalSpy.takeLast();
        QCOMPARE(m_media->totalTime(), castQVariantToInt64(args.at(0)));
    } else {
        QFAIL("Still in LoadingState after a stateChange signal was emitted. Cannot go on.");
    }

    Path path = createPath(m_media, audioOutput);
    QVERIFY(path.isValid());


    QCOMPARE(m_media->outputPaths().size(), 1);
    QCOMPARE(audioOutput->inputPaths().size(), 1);

}

void tst_MediaObject::checkForDefaults()
{
    QCOMPARE(m_media->tickInterval(), qint32(0));
    QCOMPARE(m_media->prefinishMark(), qint32(0));
}

void tst_MediaObject::stopToStop()
{
    QCOMPARE(m_stateChangedSignalSpy->count(), 0);
    QCOMPARE(m_media->state(), Phonon::StoppedState);
    m_media->stop();
    QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 2000);
    QCOMPARE(m_stateChangedSignalSpy->count(), 0);
    QCOMPARE(m_media->state(), Phonon::StoppedState);
}

void tst_MediaObject::stopToPause()
{
    QCOMPARE(m_stateChangedSignalSpy->count(), 0);
    QCOMPARE(m_media->state(), Phonon::StoppedState);
    m_media->pause();
    if (m_stateChangedSignalSpy->isEmpty()) {
        QVERIFY(QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 6000));
    }
    QCOMPARE(m_stateChangedSignalSpy->count(), 1);
    QCOMPARE(m_media->state(), Phonon::PausedState);
}

void tst_MediaObject::stopToPlay()
{
    startPlayback();
    QTest::waitForSignal(m_media, SIGNAL(finished()), 1000);
    stopPlayback(Phonon::PlayingState);
}

void tst_MediaObject::playToPlay()
{
    startPlayback();

    m_media->play();
    QCOMPARE(m_stateChangedSignalSpy->count(), 0);
    QCOMPARE(m_media->state(), Phonon::PlayingState);

    stopPlayback(Phonon::PlayingState);
}

void tst_MediaObject::playToPause()
{
    startPlayback();
    QCOMPARE(m_media->state(), Phonon::PlayingState);
    pausePlayback();
    stopPlayback(Phonon::PausedState);
}

void tst_MediaObject::playToStop()
{
    startPlayback();
    stopPlayback(Phonon::PlayingState);
}

void tst_MediaObject::pauseToPause()
{
    startPlayback();
    pausePlayback();

    m_media->pause();
    QCOMPARE(m_stateChangedSignalSpy->count(), 0);
    QCOMPARE(m_media->state(), Phonon::PausedState);

    stopPlayback(Phonon::PausedState);
}

void tst_MediaObject::pauseToPlay()
{
    startPlayback();
    pausePlayback();
    startPlayback2(Phonon::PausedState);
    stopPlayback(Phonon::PlayingState);
}

void tst_MediaObject::pauseToStop()
{
    startPlayback();
    pausePlayback();
    stopPlayback(Phonon::PausedState);
}

/*!

    We attempt to play a SDP file. An SDP file essentially describes different
    media streams and is hence a layer in front of the actual media(s).
    Sometimes the backend handles the SDP file, in other cases not.

    Some Phonon backends doesn't support SDP at all, ifdef appropriately. Real
    Player and Helix, the two backends for Symbian, are known to support SDP.
 */
void tst_MediaObject::playSDP()
{
#ifdef Q_OS_SYMBIAN
    QString sdpFile;
    copyMediaFile(QLatin1String(":/media/test.sdp"), QLatin1String("test.sdp"), sdpFile);

    // Let's verify our test setup.
    QVERIFY(QFileInfo(sdpFile).isReadable());

    // We need a window in order to setup the video.
    QWidget widget;
    widget.show();

    const MediaSource oldSource(m_media->currentSource());
    const MediaSource sdpSource(sdpFile);
    m_media->setCurrentSource(sdpSource);
    if (m_media->state() != Phonon::StoppedState)
        QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 10000);

    // At this point we're in error state due to absent media, but it has now loaded the SDP:
    QCOMPARE(m_media->errorString(), QString::fromLatin1("Buffering clip failed: Unknown error (-39)"));

    // We cannot play the SDP, we can neither attempt to play it, because we
    // won't get a state change from ErrorState to ErrorState, and hence block
    // on a never occuring signal.
    m_media->setCurrentSource(oldSource);

#else
    QSKIP("Unsupported on this platform.", SkipAll);
#endif
}

void tst_MediaObject::testPrefinishMark()
{
    const qint32 requestedPrefinishMarkTime = 2000;
    m_media->setPrefinishMark(requestedPrefinishMarkTime);
    QCOMPARE(m_media->prefinishMark(), requestedPrefinishMarkTime);
    QSignalSpy prefinishMarkReachedSpy(m_media, SIGNAL(prefinishMarkReached(qint32)));
    QSignalSpy finishSpy(m_media, SIGNAL(finished()));
    startPlayback();
    if (m_media->isSeekable()) {
        m_media->seek(m_media->totalTime() - SEEK_BACKWARDS - requestedPrefinishMarkTime); // give it 4 seconds
    }
    int wait = 10000;
    int total = 0;
    while (prefinishMarkReachedSpy.count() == 0 && (m_media->state() == Phonon::PlayingState ||
                m_media->state() == Phonon::BufferingState)) {
        wait = qMax(1000, wait / 2);
        QTest::waitForSignal(m_media, SIGNAL(prefinishMarkReached(qint32)), wait);
        total += wait;
        if (total >= 60*1000) // we wait 1 minute
            QFAIL("Timeout failure waiting for signal");
    }
    // at this point the media should be about to finish playing
    qint64 r = m_media->remainingTime();
    Phonon::State state = m_media->state();
    QCOMPARE(prefinishMarkReachedSpy.count(), 1);
    const qint32 prefinishMark = castQVariantToInt32(prefinishMarkReachedSpy.first().at(0));
    QVERIFY(prefinishMark <= requestedPrefinishMarkTime + 150); // allow it to be up to 150ms too early
    if (state == Phonon::PlayingState || state == Phonon::BufferingState) {
        if (r > prefinishMark) {
            qDebug() << "remainingTime =" << r;
            QFAIL("remainingTime needs to be less than or equal to prefinishMark");
        }
        QVERIFY(r <= prefinishMark);
        QTest::waitForSignal(m_media, SIGNAL(finished()), 10000);
    } else {
        QVERIFY(prefinishMark >= 0);
    }
    QCOMPARE(finishSpy.count(), 1);
}

void tst_MediaObject::enqueueMedia()
{
    m_media->enqueue(m_url);
}

Q_DECLARE_METATYPE(Phonon::MediaSource)
void tst_MediaObject::testJustInTimeQueuing()
{
#ifdef Q_OS_WINCE
    QSKIP("crashes on Windows CE", SkipAll);
#endif
    qRegisterMetaType<Phonon::MediaSource>("Phonon::MediaSource");
    QSignalSpy currentSourceChanged(m_media, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)));
    QSignalSpy finished(m_media, SIGNAL(finished()));
    connect(m_media, SIGNAL(aboutToFinish()), SLOT(enqueueMedia()));

    startPlayback();
    if (m_media->isSeekable()) {
        m_media->seek(m_media->totalTime() - SEEK_BACKWARDS);
        QVERIFY(QTest::waitForSignal(m_media, SIGNAL(aboutToFinish()), 6000));
    } else {
        QVERIFY(QTest::waitForSignal(m_media, SIGNAL(aboutToFinish()), 3000 + m_media->remainingTime()));
    }
    disconnect(m_media, SIGNAL(aboutToFinish()), this, SLOT(enqueueMedia()));
    if (currentSourceChanged.isEmpty()) {
        QVERIFY(QTest::waitForSignal(m_media, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)), 3000));
    }
    QCOMPARE(currentSourceChanged.size(), 1);
    QCOMPARE(finished.size(), 0);
    QVERIFY(m_media->queue().isEmpty());
    stopPlayback(m_media->state());
}

void tst_MediaObject::testPauseOnFinish()
{
    startPlayback();
    QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 1000);
    QCOMPARE(m_media->state(), Phonon::PlayingState);
    if (m_media->isSeekable() && m_media->totalTime() > 2000)
        m_media->seek(m_media->totalTime() - 2000);
    QTest::waitForSignal(m_media, SIGNAL(finished()), 4000);
    QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 1000);

    QCOMPARE(m_media->state(), Phonon::PausedState);
    connect(m_media, SIGNAL(finished()), m_media, SLOT(stop()));
    m_media->seek(m_media->totalTime() - 2000);
    m_media->play();

    QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 1000);
    QCOMPARE(m_media->state(), Phonon::PlayingState);
    QTest::waitForSignal(m_media, SIGNAL(finished()), 4000);
    QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 1000);
    stopPlayback(Phonon::StoppedState);
}

void tst_MediaObject::testReconnectBetweenTwoMediaObjects(){
    // Purpose: Test that phonon can handle swithing the same sink
    // between different media objects.

    Phonon::MediaObject obj1;
    Phonon::MediaObject obj2;
    Phonon::AudioOutput audio1;
    Phonon::Path p1 = Phonon::createPath(&obj1, &audio1);
    QVERIFY(p1.isValid());

    QVERIFY(p1.reconnect(&obj1, &audio1));
    QVERIFY(p1.isValid());
    QVERIFY(p1.reconnect(&obj2, &audio1));
    QVERIFY(p1.isValid());
    QVERIFY(p1.reconnect(&obj1, &audio1));
    QVERIFY(p1.isValid());

    // Repeat the same test while playing:
    QFile file(MEDIA_FILEPATH);
    obj1.setCurrentSource(&file);
    QFile file2(MEDIA_FILEPATH);
    obj2.setCurrentSource(&file2);
    obj1.play();
    obj2.play();
    
    QVERIFY(p1.reconnect(&obj1, &audio1));
    QVERIFY(p1.isValid());
    QVERIFY(p1.reconnect(&obj2, &audio1));
    QVERIFY(p1.isValid());
    QVERIFY(p1.reconnect(&obj1, &audio1));
    QVERIFY(p1.isValid());
}

void tst_MediaObject::testPlayOnFinish()
{
    connect(m_media, SIGNAL(finished()), SLOT(setMediaAndPlay()));
    startPlayback();
    if (m_media->isSeekable()) {
        m_media->seek(m_media->totalTime() - SEEK_BACKWARDS);
        QVERIFY(QTest::waitForSignal(this, SIGNAL(continueTestPlayOnFinish()), 6000));
    } else {
        QVERIFY(QTest::waitForSignal(this, SIGNAL(continueTestPlayOnFinish()), 3000 + m_media->remainingTime()));
    }
    QTest::waitForSignal(m_media, SIGNAL(finished()), 1000);
    stopPlayback(m_media->state());
}

void tst_MediaObject::testTickSignal()
{
    QTime start1;
    QTime start2;
#ifdef Q_OS_WINCE //On Windows CE we only provide ticks above 400ms
    for (qint32 tickInterval = 400; tickInterval <= 1000; tickInterval *= 2)
#else
    for (qint32 tickInterval = 80; tickInterval <= 500; tickInterval *= 2)
#endif
    {
        QSignalSpy tickSpy(m_media, SIGNAL(tick(qint64)));
        //qDebug() << "Test 20 ticks with an interval of" <<  tickInterval << "ms";
        m_media->setTickInterval(tickInterval);
        QVERIFY(m_media->tickInterval() <= tickInterval);
        QVERIFY(m_media->tickInterval() >= tickInterval/2);
        QVERIFY(tickSpy.isEmpty());
        m_media->seek(0); //let's go back to the beginning
        start1.start();
        startPlayback();
        start2.start();
        int lastCount = 0;
        qint64 s1, s2 = start2.elapsed();
        while (tickSpy.count() < 20 && (m_media->state() == Phonon::PlayingState || m_media->state() == Phonon::BufferingState))
        {
            if (tickSpy.count() > lastCount)
            {
                s1 = start1.elapsed();
                qint64 tickTime = castQVariantToInt64(tickSpy.last().at(0));
                lastCount = tickSpy.count();
                // s1 is the time from before the beginning of the playback to
                // after the tick signal
                // s2 is the time from after the beginning of the playback to
                // before the tick signal
                // so: s2 <= s1
                QVERIFY(tickTime <= m_media->currentTime());
                if (s1 + ALLOWED_TICK_INACCURACY < tickTime || s2 - ALLOWED_TICK_INACCURACY > tickTime) {
                    qDebug()
                        << "\n" << lastCount << "ticks have been received"
                        << "\ntime from before playback was started to after the tick signal was received:" << s1 << "ms"
                        << "\ntime from after playback was started to before the tick signal was received:" << s2 << "ms"
                        << "\nreported tick time:" << tickTime << "ms"
                        << "\nallowed inaccuracy: +/-" << ALLOWED_TICK_INACCURACY << "ms";
                    for (int i = 0; i < tickSpy.count(); ++i) {
                        qDebug() << castQVariantToInt64(tickSpy[i].at(0));
                    }
                }
                QVERIFY(s1 + ALLOWED_TICK_INACCURACY >= tickTime);
                QVERIFY(s2 - ALLOWED_TICK_INACCURACY <= tickTime);
#ifndef Q_OS_WINCE
                QVERIFY(s1 >= lastCount * m_media->tickInterval());
#else
                QVERIFY(s1 >= lastCount * m_media->tickInterval() - ALLOWED_TICK_INACCURACY);
#endif
                if (s2 > (lastCount + 1) * m_media->tickInterval())
                    QWARN(qPrintable(QString("%1. tick came too late: %2ms elapsed while this tick should have come before %3ms")
                            .arg(lastCount).arg(s2).arg((lastCount + 1) * m_media->tickInterval())));
            } else if (lastCount == 0 && s2 > 20 * m_media->tickInterval()) {
                QFAIL("no tick signals are being received");
            }
            s2 = start2.elapsed();
            QTest::waitForSignal(m_media, SIGNAL(tick(qint64)), 2000);
        }
#ifndef Q_OS_WINCE //the shorter wave file is finished on Windows CE...
        stopPlayback(Phonon::PlayingState);
#else
        stopPlayback(m_media->state());
#endif
    }
}

void tst_MediaObject::testSeek()
{
    m_media->seek(0); // let's seek back to the beginning
    startPlayback();
    QTime timer;
    timer.start();
    qint64 t = m_media->totalTime();
    qint64 c = m_media->currentTime();
    qint64 r = m_media->remainingTime();
    int elapsed = timer.elapsed();
    if (c + r > t + elapsed || c + r < t - elapsed) {
    //  qDebug() << "currentTime:" << c
    //      << "remainingTime:" << r
    //      << "totalTime:" << t;
        QFAIL("currentTime + remainingTime doesn't come close enough to totalTime");
    }

    QVERIFY(c + r <= t + elapsed);
    QVERIFY(c + r >= t - elapsed);
    if (m_media->isSeekable())
        if (r > 0)
        {
            m_media->setTickInterval(20);
            qint64 s = c + r / 2;
            testOneSeek(s);

            s /= 2;
            testOneSeek(s);
            s = s * 3 / 2;
            testOneSeek(s);

            pausePlayback();

            s = s * 3 / 2;
            testOneSeek(s);
            s /= 2;
            testOneSeek(s);

            m_media->setTickInterval(0);


            stopPlayback(Phonon::PausedState);
            return;
        }
        else
            QWARN("didn't test seeking as the MediaObject reported a remaining size <= 0");
    else
        QWARN("didn't test seeking as the MediaObject is not seekable");
    stopPlayback(Phonon::PlayingState);
}


void tst_MediaObject::setMediaAndPlay()
{
    m_stateChangedSignalSpy->clear();
    QCOMPARE(m_stateChangedSignalSpy->count(), 0);

    QSignalSpy totalTimeChangedSignalSpy(m_media, SIGNAL(totalTimeChanged(qint64)));
    QVERIFY(m_media->currentSource().type() != MediaSource::Invalid);
    Phonon::State state = m_media->state();
    QVERIFY(state == Phonon::StoppedState || state == Phonon::PlayingState || Phonon::PausedState);
    m_media->setCurrentSource(m_url);
    // before calling play() we better make sure that if play() finishes very fast that we don't get
    // called again
    disconnect(m_media, SIGNAL(finished()), this, SLOT(setMediaAndPlay()));
    state = m_media->state();
    startPlayback2(state);

    emit continueTestPlayOnFinish();
}

void tst_MediaObject::testPlayBeforeFinish()
{
    startPlayback();
    QCOMPARE(m_stateChangedSignalSpy->size(), 0);
    Phonon::State state = m_media->state();
    QCOMPARE(state, Phonon::PlayingState);
    m_media->setCurrentSource(m_url);
    m_media->play();
    if (m_stateChangedSignalSpy->isEmpty()) {
        QVERIFY(QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 4000));
    }
    // first (optional) state to reach is StoppedState
    QList<QVariant> args = m_stateChangedSignalSpy->takeFirst();
    Phonon::State oldstate = qvariant_cast<Phonon::State>(args.at(1));
    QCOMPARE(oldstate, state);
    state = qvariant_cast<Phonon::State>(args.at(0));
    if (state == Phonon::StoppedState) {
        if (m_stateChangedSignalSpy->isEmpty()) {
            QVERIFY(QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 4000));
        }
        args = m_stateChangedSignalSpy->takeFirst();
        oldstate = qvariant_cast<Phonon::State>(args.at(1));
        QCOMPARE(oldstate, state);
        state = qvariant_cast<Phonon::State>(args.at(0));
    }
    // next LoadingState
    QCOMPARE(state, Phonon::LoadingState);
    if (m_stateChangedSignalSpy->isEmpty()) {
        QVERIFY(QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 4000));
    }
    // next either BufferingState or PlayingState
    args = m_stateChangedSignalSpy->takeFirst();
    oldstate = qvariant_cast<Phonon::State>(args.at(1));
    QCOMPARE(oldstate, state);
    state = qvariant_cast<Phonon::State>(args.at(0));
    if (state == Phonon::BufferingState) {
        if (m_stateChangedSignalSpy->isEmpty()) {
            QVERIFY(QTest::waitForSignal(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 4000)); // buffering can take a while
        }
        args = m_stateChangedSignalSpy->takeFirst();
        oldstate = qvariant_cast<Phonon::State>(args.at(1));
        QCOMPARE(oldstate, state);
        state = qvariant_cast<Phonon::State>(args.at(0));
    }
#ifdef Q_WS_MAC
    // m_media->setCurrentSource(m_url) in phonon frontend will always call
    // 'stop' on the backend before calling 'setSource'. So the QT7 backend
    // goes into stop, and naturally remains there after setting the new source.
    // So going into playing state cannot happend when the backend is synchronized.
    // Thats the reason for the ifdef.
    QCOMPARE(state, Phonon::StoppedState);
#else
    stopPlayback(Phonon::PlayingState);
#endif
}

void tst_MediaObject::cleanupTestCase()
{
    if (m_stateChangedSignalSpy)
      delete m_stateChangedSignalSpy;
    if (m_media)
      delete m_media;
#ifdef Q_OS_WINCE
    QTest::qWait(200);
#endif
    if (!m_tmpFileName.isNull()) {
        QVERIFY(QFile::remove(m_tmpFileName));
    }
}

void tst_MediaObject::_testOneSeek(qint64 seekTo)
{
   qint64 t = m_media->totalTime();
    qint64 oldTime = m_media->currentTime();
    if (oldTime == seekTo) {
        return;
    }

    QTime seekDuration;
    seekDuration.start();
    m_media->seek(seekTo);

    QVERIFY(oldTime == 0 || seekTo == 0 || m_media->currentTime() != 0);

    int bufferingTime = 0;
    Phonon::State s = m_media->state();
    QTime timer;
    if (s == Phonon::BufferingState) {
        timer.start();
    }
    QEventLoop loop;
    connect(m_media, SIGNAL(tick(qint64)), &loop, SLOT(quit()));
    connect(m_media, SIGNAL(stateChanged(Phonon::State,Phonon::State)), &loop, SLOT(quit()));

    qint64 c = m_media->currentTime();
    qint64 r = m_media->remainingTime();
    int elapsed = 0;
    while (qAbs(c - seekTo) > ALLOWED_SEEK_INACCURACY){
        QTimer::singleShot(ALLOWED_TIME_FOR_SEEKING, &loop, SLOT(quit()));

        loop.exec();
        c = m_media->currentTime();
        r = m_media->remainingTime();
        if (s == Phonon::BufferingState) {
            bufferingTime += timer.restart();
        } else {
            timer.start();
        }
        s = m_media->state();
        elapsed = seekDuration.elapsed();
        QVERIFY(elapsed - bufferingTime < (ALLOWED_TIME_FOR_SEEKING + SEEKING_TOLERANCE));
    }

    QVERIFY(c >= seekTo - ALLOWED_SEEK_INACCURACY);
    if (s == Phonon::PausedState) {
        QVERIFY(bufferingTime == 0);
        elapsed = 0;
    }
    if (c > seekTo + ALLOWED_SEEK_INACCURACY + elapsed - bufferingTime) {
        QFAIL("currentTime is greater than the requested time + the time that elapsed since the seek started.");
    }
    if (c + r > t + 200 || c + r < t - 200) {
        QFAIL("currentTime + remainingTime doesn't come close enough to totalTime");
    }
    m_success = true;
}

void tst_MediaObject::volumeSliderMuteVisibility()
{
    //this test doesn't really belong to mediaobject
    // ### see if we should create a realy Phonon::VolumeSlider autotest
    Phonon::VolumeSlider slider;
    QVERIFY(slider.isMuteVisible()); // that is the default value
    slider.setMuteVisible(true);
    QVERIFY(slider.isMuteVisible());

    //let's check that changing the visibility of the slider itself
    //doesn't change what the slider reports
    slider.setVisible(false);
    QVERIFY(slider.isMuteVisible());
    slider.setVisible(true);

    slider.setMuteVisible(false);
    QVERIFY(!slider.isMuteVisible());
    slider.setMuteVisible(true);
    QVERIFY(slider.isMuteVisible());
}


#endif //QT_NO_PHONON


QTEST_MAIN(tst_MediaObject)

#include "tst_mediaobject.moc"
// vim: sw=4 ts=4
