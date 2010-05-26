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
#include <qaudiodeviceinfo.h>

#include <QStringList>
#include <QList>


class tst_QAudioDeviceInfo : public QObject
{
    Q_OBJECT
public:
    tst_QAudioDeviceInfo(QObject* parent=0) : QObject(parent) {}

private slots:
    void initTestCase();
    void checkAvailableDefaultInput();
    void checkAvailableDefaultOutput();
    void outputList();
    void codecs();
    void channels();
    void sampleSizes();
    void byteOrders();
    void sampleTypes();
    void frequencies();
    void isformat();
    void preferred();

private:
    bool              available;
    QAudioDeviceInfo* device;
};

void tst_QAudioDeviceInfo::initTestCase()
{
    // Only perform tests if audio output device exists!
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if(devices.size() > 0)
        available = true;
    else {
        qWarning()<<"NOTE: no audio output device found, no test will be performed";
        available = false;
    }
}

void tst_QAudioDeviceInfo::checkAvailableDefaultInput()
{
    // Only perform tests if audio input device exists!
    bool storeAvailable = available;
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    if(devices.size() > 0)
        available = true;
    else {
        qWarning()<<"NOTE: no audio input device found, no test will be performed";
        available = false;
    }
    if(available)
        QVERIFY(!QAudioDeviceInfo::defaultInputDevice().isNull());
    available = storeAvailable;
}

void tst_QAudioDeviceInfo::checkAvailableDefaultOutput()
{
    if(available)
        QVERIFY(!QAudioDeviceInfo::defaultOutputDevice().isNull());
}

void tst_QAudioDeviceInfo::outputList()
{
    if(available) {
        QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
        QVERIFY(devices.size() > 0);
        device = new QAudioDeviceInfo(devices.at(0));
    }
}

void tst_QAudioDeviceInfo::codecs()
{
    if(available) {
        QStringList avail = device->supportedCodecs();
        QVERIFY(avail.size() > 0);
    }
}

void tst_QAudioDeviceInfo::channels()
{
    if(available) {
        QList<int> avail = device->supportedChannels();
        QVERIFY(avail.size() > 0);
    }
}

void tst_QAudioDeviceInfo::sampleSizes()
{
    if(available) {
        QList<int> avail = device->supportedSampleSizes();
        QVERIFY(avail.size() > 0);
    }
}

void tst_QAudioDeviceInfo::byteOrders()
{
    if(available) {
        QList<QAudioFormat::Endian> avail = device->supportedByteOrders();
        QVERIFY(avail.size() > 0);
    }
}

void tst_QAudioDeviceInfo::sampleTypes()
{
    if(available) {
        QList<QAudioFormat::SampleType> avail = device->supportedSampleTypes();
        QVERIFY(avail.size() > 0);
    }
}

void tst_QAudioDeviceInfo::frequencies()
{
    if(available) {
        QList<int> avail = device->supportedFrequencies();
        QVERIFY(avail.size() > 0);
    }
}

void tst_QAudioDeviceInfo::isformat()
{
    if(available) {
        QAudioFormat     format;
        format.setFrequency(44100);
        format.setChannels(2);
        format.setSampleType(QAudioFormat::SignedInt);
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");

        // Should always be true for these format
        QVERIFY(device->isFormatSupported(format));
    }
}

void tst_QAudioDeviceInfo::preferred()
{
    if(available) {
        QAudioFormat format = device->preferredFormat();
        QVERIFY(format.frequency() == 44100);
        QVERIFY(format.channels() == 2);
    }
}

QTEST_MAIN(tst_QAudioDeviceInfo)

#include "tst_qaudiodeviceinfo.moc"
