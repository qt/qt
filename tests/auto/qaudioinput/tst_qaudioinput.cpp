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
#include <qaudioinput.h>
#include <qaudiodeviceinfo.h>
#include <qaudio.h>
#include <qaudioformat.h>

#if defined(Q_OS_SYMBIAN)
#define SRCDIR ""
#endif

Q_DECLARE_METATYPE(QAudioFormat)

class tst_QAudioInput : public QObject
{
    Q_OBJECT
public:
    tst_QAudioInput(QObject* parent=0) : QObject(parent) {}

private slots:
    void initTestCase();
    void invalidFormat_data();
    void invalidFormat();
    void settings();
    void buffers();
    void notifyInterval();
    void pullFile();

private:
    bool           available;
    QAudioFormat   format;
    QAudioInput*   audio;
};

void tst_QAudioInput::initTestCase()
{
    qRegisterMetaType<QAudioFormat>();

    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    // Only perform tests if audio input device exists!
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    if(devices.size() > 0)
        available = true;
    else {
        qWarning()<<"NOTE: no audio input device found, no test will be performed";
        available = false;
    }

    if(available)
        audio = new QAudioInput(format, this);
}

void tst_QAudioInput::invalidFormat_data()
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

void tst_QAudioInput::invalidFormat()
{
    QFETCH(QAudioFormat, invalidFormat);

    QAudioInput audioInput(invalidFormat, this);

    // Check that we are in the default state before calling start
    QVERIFY2((audioInput.state() == QAudio::StoppedState), "state() was not set to StoppedState before start()");
    QVERIFY2((audioInput.error() == QAudio::NoError), "error() was not set to QAudio::NoError before start()");

    audioInput.start();

    // Check that error is raised
    QVERIFY2((audioInput.error() == QAudio::OpenError),"error() was not set to QAudio::OpenError after start()");
}

void tst_QAudioInput::settings()
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

void tst_QAudioInput::buffers()
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

void tst_QAudioInput::notifyInterval()
{
    if(available) {
        QVERIFY(audio->notifyInterval() == 1000);   // Default

        audio->setNotifyInterval(500);
        QVERIFY(audio->notifyInterval() == 500);    // Custom

        audio->setNotifyInterval(1000);             // reset
    }
}

void tst_QAudioInput::pullFile()
{
    if(available) {
        QFile filename(SRCDIR"test.raw");
        filename.open( QIODevice::WriteOnly | QIODevice::Truncate );

        QSignalSpy readSignal(audio, SIGNAL(notify()));
        QSignalSpy stateSignal(audio, SIGNAL(stateChanged(QAudio::State)));

        // Always have default states, before start
        QVERIFY(audio->state() == QAudio::StoppedState);
        QVERIFY(audio->error() == QAudio::NoError);
        QVERIFY(audio->elapsedUSecs() == 0);

        audio->start(&filename);
        QTest::qWait(20);
        // Check state and periodSize() are valid non-zero values.
        QVERIFY(audio->state() == QAudio::ActiveState);
        QVERIFY(audio->error() == QAudio::NoError);
        QVERIFY(audio->elapsedUSecs() > 10000 && audio->elapsedUSecs() < 800000);
        QVERIFY(audio->periodSize() > 0);
        QVERIFY(stateSignal.count() == 1); // State changed to QAudio::ActiveState

        // Wait until finished...
        QTest::qWait(5000);

        QVERIFY(readSignal.count() > 0);
        QVERIFY(audio->processedUSecs() > 0);

        audio->stop();
        QTest::qWait(20);
        QVERIFY(audio->state() == QAudio::StoppedState);
        QVERIFY(audio->elapsedUSecs() == 0);
        // Can only check to make sure we got at least 1 more signal, but can be more.
        QVERIFY(stateSignal.count() > 1);

        filename.close();
    }
}

QTEST_MAIN(tst_QAudioInput)

#include "tst_qaudioinput.moc"
