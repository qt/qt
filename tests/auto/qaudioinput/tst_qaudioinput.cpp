/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtCore/qlocale.h>
#include <qaudioinput.h>
#include <qaudiodeviceinfo.h>
#include <qaudio.h>
#include <qaudioformat.h>


class tst_QAudioInput : public QObject
{
    Q_OBJECT
public:
    tst_QAudioInput(QObject* parent=0) : QObject(parent) {}

private slots:
    void initTestCase();
    void settings();
    void notifyInterval();
    void pullFile();

private:
    QAudioFormat   format;
    QAudioInput*   audio;
};

void tst_QAudioInput::initTestCase()
{
    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    audio = new QAudioInput(format, this);
}

void tst_QAudioInput::settings()
{
    QAudioFormat f = audio->format();

    QVERIFY(format.channels() == f.channels());
    QVERIFY(format.frequency() == f.frequency());
    QVERIFY(format.sampleSize() == f.sampleSize());
    QVERIFY(format.codec() == f.codec());
    QVERIFY(format.byteOrder() == f.byteOrder());
    QVERIFY(format.sampleType() == f.sampleType());
}

void tst_QAudioInput::notifyInterval()
{
    QVERIFY(audio->notifyInterval() == 1000);   // Default

    audio->setNotifyInterval(500);
    QVERIFY(audio->notifyInterval() == 500);    // Custom

    audio->setNotifyInterval(1000);             // reset
}

void tst_QAudioInput::pullFile()
{
    QFile filename(SRCDIR "test.raw");
    filename.open( QIODevice::WriteOnly | QIODevice::Truncate );

    QSignalSpy readSignal(audio, SIGNAL(notify()));
    audio->start(&filename);

    QTest::qWait(5000);

    QVERIFY(readSignal.count() > 0);
    QVERIFY(audio->totalTime() > 0);

    audio->stop();
    filename.close();
}

QTEST_MAIN(tst_QAudioInput)

#include "tst_qaudioinput.moc"
