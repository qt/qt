/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
#include <qaudioformat.h>

#if defined(Q_OS_SYMBIAN)
#define SRCDIR ""
#endif

Q_DECLARE_METATYPE(QAudioFormat)

class tst_QAudioOutput : public QObject
{
    Q_OBJECT
public:
    tst_QAudioOutput(QObject* parent=0) : QObject(parent) {}

private slots:
    void initTestCase();
    void invalidFormat_data();
    void invalidFormat();
    void settings();
    void buffers();
    void notifyInterval();
    void pullFile();
    void pushFile();

private:
    bool          available;
    QAudioFormat  format;
    QAudioOutput* audio;
};

void tst_QAudioOutput::initTestCase()
{
    qRegisterMetaType<QAudioFormat>();

    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    // Only perform tests if audio output device exists!
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if(devices.size() > 0)
        available = true;
    else {
        qWarning()<<"NOTE: no audio output device found, no test will be performed";
        available = false;
    }
    audio = new QAudioOutput(format, this);
}

void tst_QAudioOutput::invalidFormat_data()
{
    QTest::addColumn<QAudioFormat>("invalidFormat");

    QAudioFormat audioFormat;

    QTest::newRow("Null Format")
            << audioFormat;

    audioFormat = format;
    audioFormat.setChannels(0);
    QTest::newRow("Channel count 0")
            << audioFormat;

    audioFormat = format;
    audioFormat.setFrequency(0);
    QTest::newRow("Sample rate 0")
            << audioFormat;

    audioFormat = format;
    audioFormat.setSampleSize(0);
    QTest::newRow("Sample size 0")
            << audioFormat;
}

void tst_QAudioOutput::invalidFormat()
{
    QFETCH(QAudioFormat, invalidFormat);

    QAudioOutput audioOutput(invalidFormat, this);

    // Check that we are in the default state before calling start
    QVERIFY2((audioOutput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioOutput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    audioOutput.start();

    // Check that error is raised
    QVERIFY2((audioOutput.error() == QAudio::OpenError),"error() was not set to QAudio::OpenError after start()");
}

void tst_QAudioOutput::settings()
{
    if(available) {
        // Confirm the setting we added in the init function.
        QAudioFormat f = audio->format();

        QVERIFY(format.channels() == f.channels());
        QVERIFY(format.frequency() == f.frequency());
        QVERIFY(format.sampleSize() == f.sampleSize());
        QVERIFY(format.codec() == f.codec());
        QVERIFY(format.byteOrder() == f.byteOrder());
        QVERIFY(format.sampleType() == f.sampleType());
    }
}

void tst_QAudioOutput::buffers()
{
    if(available) {
        // Should always have a buffer size greater than zero.
        int store = audio->bufferSize();
        audio->setBufferSize(4096);
        QVERIFY(audio->bufferSize() > 0);
        audio->setBufferSize(store);
        QVERIFY(audio->bufferSize() == store);
    }
}

void tst_QAudioOutput::notifyInterval()
{
    if(available) {
        QVERIFY(audio->notifyInterval() == 1000);   // Default

        audio->setNotifyInterval(500);
        QVERIFY(audio->notifyInterval() == 500);    // Custom

        audio->setNotifyInterval(1000);             // reset
    }
}

void tst_QAudioOutput::pullFile()
{
    if(available) {
        QFile file(SRCDIR"4.wav");
        QVERIFY(file.exists());
        file.open(QIODevice::ReadOnly);

        QSignalSpy readSignal(audio, SIGNAL(notify()));
        QSignalSpy stateSignal(audio, SIGNAL(stateChanged(QAudio::State)));
        audio->setNotifyInterval(100);

        // Always have default states, before start
        QVERIFY(audio->state() == QAudio::StoppedState);
        QVERIFY(audio->error() == QAudio::NoError);
        QVERIFY(audio->elapsedUSecs() == 0);

        audio->start(&file);
        QTest::qWait(20); // wait 20ms
        // Check state, bytesFree() and periodSize() are valid non-zero values.
        QVERIFY(audio->state() == QAudio::ActiveState);
        QVERIFY(audio->error() == QAudio::NoError);
        QVERIFY(audio->periodSize() > 0);
        QVERIFY(audio->elapsedUSecs() > 10000 && audio->elapsedUSecs() < 800000);
        QVERIFY(stateSignal.count() == 1); // State changed to QAudio::ActiveState

        // Wait until finished...
        QTestEventLoop::instance().enterLoop(1);
        QCOMPARE(audio->processedUSecs(), qint64(692250));

#ifdef Q_OS_WINCE
        // 4.wav is a little less than 700ms, so notify should fire 4 times on Wince!
        QVERIFY(readSignal.count() >= 4);
#else
        // 4.wav is a little less than 700ms, so notify should fire 6 times!
        QVERIFY(readSignal.count() >= 6);
#endif
        audio->stop();
        QTest::qWait(20); // wait 20ms
        QVERIFY(audio->state() == QAudio::StoppedState);
        QVERIFY(audio->elapsedUSecs() == 0);
        // Can only check to make sure we got at least 1 more signal, but can be more.
        QVERIFY(stateSignal.count() > 1);

        file.close();
    }
}

void tst_QAudioOutput::pushFile()
{
    if(available) {
        QFile file(SRCDIR"4.wav");
        QVERIFY(file.exists());
        file.open(QIODevice::ReadOnly);

        const qint64 fileSize = file.size();

        QIODevice* feed = audio->start();

        char* buffer = new char[fileSize];
        file.read(buffer, fileSize);

        qint64 counter=0;
        qint64 written=0;
        while(written < fileSize) {
            written+=feed->write(buffer+written,fileSize-written);
            QTest::qWait(20);
            counter++;
        }
        QTestEventLoop::instance().enterLoop(1);

        QVERIFY(written == fileSize);
        QVERIFY(audio->processedUSecs() == 692250);

        audio->stop();
        file.close();
        delete [] buffer;
        delete audio;
    }
}

QTEST_MAIN(tst_QAudioOutput)

#include "tst_qaudiooutput.moc"
