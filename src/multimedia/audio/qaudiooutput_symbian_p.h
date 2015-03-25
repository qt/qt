/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QAUDIOOUTPUT_SYMBIAN_P_H
#define QAUDIOOUTPUT_SYMBIAN_P_H

#include <QtMultimedia/qaudioengine.h>
#include <QTime>
#include <QTimer>
#include <sounddevice.h>
#include "qaudio_symbian_p.h"

QT_BEGIN_NAMESPACE

class QAudioOutputPrivate;

class SymbianAudioOutputPrivate : public QIODevice
{
    friend class QAudioOutputPrivate;
    Q_OBJECT
public:
    SymbianAudioOutputPrivate(QAudioOutputPrivate *audio);
    ~SymbianAudioOutputPrivate();

    qint64 readData(char *data, qint64 len);
    qint64 writeData(const char *data, qint64 len);

private:
    QAudioOutputPrivate *const m_audioDevice;
};

class QAudioOutputPrivate
    :   public QAbstractAudioOutput
{
    friend class SymbianAudioOutputPrivate;
    Q_OBJECT
public:
    QAudioOutputPrivate(const QByteArray &device,
                       const QAudioFormat &audioFormat);
    ~QAudioOutputPrivate();

    // QAbstractAudioOutput
    QIODevice* start(QIODevice *device = 0);
    void stop();
    void reset();
    void suspend();
    void resume();
    int bytesFree() const;
    int periodSize() const;
    void setBufferSize(int value);
    int bufferSize() const;
    void setNotifyInterval(int milliSeconds);
    int notifyInterval() const;
    qint64 processedUSecs() const;
    qint64 elapsedUSecs() const;
    QAudio::Error error() const;
    QAudio::State state() const;
    QAudioFormat format() const;

private slots:
    void dataReady();
    void underflowTimerExpired();
    void devsoundInitializeComplete(int err);
    void devsoundBufferToBeFilled(CMMFBuffer *);
    void devsoundPlayError(int err);

private:
   void open();
   void startPlayback();
   void writePaddingData();
   qint64 pushData(const char *data, qint64 len);
   void pullData();
   void bufferFilled();
   void lastBufferFilled();
   Q_INVOKABLE void close();

   qint64 getSamplesPlayed() const;

   void setError(QAudio::Error error);
   void setState(SymbianAudio::State state);

   bool isDataReady() const;

private:
    const QByteArray m_device;
    const QAudioFormat m_format;

    int m_clientBufferSize;
    int m_notifyInterval;
    QScopedPointer<QTimer> m_notifyTimer;
    QTime m_elapsed;
    QAudio::Error m_error;

    SymbianAudio::State m_internalState;
    QAudio::State m_externalState;

    bool m_pullMode;
    QIODevice *m_source;

    SymbianAudio::DevSoundWrapper* m_devSound;

    // Buffer provided by DevSound, to be filled with data.
    CMMFDataBuffer *m_devSoundBuffer;

    int m_devSoundBufferSize;

    // Number of bytes transferred from QIODevice to QAudioOutput.  It is
    // necessary to count this because data is dropped when suspend() is
    // called.  The difference between the position reported by DevSound and
    // this value allows us to calculate m_bytesPadding;
    quint32 m_bytesWritten;

    // True if client has provided data while the audio subsystem was not
    // ready to consume it.
    bool m_pushDataReady;

    // Number of zero bytes which will be written when client calls resume().
    quint32 m_bytesPadding;

    // True if PlayError(KErrUnderflow) has been called.
    bool m_underflow;

    // True if a buffer marked with the "last buffer" flag has been provided
    // to DevSound.
    bool m_lastBuffer;

    // Some DevSound implementations ignore all underflow errors raised by the
    // audio driver, unless the last buffer flag has been set by the client.
    // In push-mode playback, this flag will never be set, so the underflow
    // error will never be reported.  In order to work around this, a timer
    // is used, which gets reset every time the client provides more data.  If
    // the timer expires, an underflow error is raised by this object.
    QScopedPointer<QTimer> m_underflowTimer;

    // Result of previous call to CMMFDevSound::SamplesPlayed().  This value is
    // used to determine whether, when m_underflowTimer expires, an
    // underflow error has actually occurred.
    quint32 m_samplesPlayed;

    // Samples played up to the last call to suspend().  It is necessary
    // to cache this because suspend() is implemented using
    // CMMFDevSound::Stop(), which resets DevSound's SamplesPlayed() counter.
    quint32 m_totalSamplesPlayed;

};

QT_END_NAMESPACE

#endif
