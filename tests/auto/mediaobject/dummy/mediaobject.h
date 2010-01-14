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

#ifndef PHONON_DUMMY_MEDIAOBJECT_H
#define PHONON_DUMMY_MEDIAOBJECT_H

#include "backend.h"
#include <phonon/mediaobjectinterface.h>
#include <phonon/addoninterface.h>

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtCore/QDate>
#include <QtCore/QEvent>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE

class QTimer;
typedef QMultiMap<QString, QString> TagMap;

namespace Phonon
{
namespace Dummy
{

class VideoWidget;
class AudioPath;
class VideoPath;
class AudioOutput;

class MediaObject : public QObject, public MediaObjectInterface
{
    friend class Stream;
    Q_OBJECT
    Q_INTERFACES(Phonon::MediaObjectInterface
    )

public:

    MediaObject(Backend *backend, QObject *parent);
    ~MediaObject();
    Phonon::State state() const;

    bool hasVideo() const;
    bool isSeekable() const;

    qint64 currentTime() const;
    qint32 tickInterval() const;

    void setTickInterval(qint32 newTickInterval);

    void play();
    void pause();
    void stop();
    void seek(qint64 time);

    QString errorString() const;
    Phonon::ErrorType errorType() const;

    QUrl url() const;
    qint64 totalTime() const;

    qint32 prefinishMark() const;
    void setPrefinishMark(qint32 newPrefinishMark);

    qint32 transitionTime() const;
    void setTransitionTime(qint32);
    qint64 remainingTime() const;

    void setSource(const MediaSource &source);
    void setNextSource(const MediaSource &source);
    MediaSource source() const;

    void saveState();
    void resumeState();

public Q_SLOTS:
    void setState(State);

Q_SIGNALS:
    void currentSourceChanged(const MediaSource &newSource);
    void stateChanged(Phonon::State newstate, Phonon::State oldstate);
    void tick(qint64 time);
    void metaDataChanged(QMultiMap<QString, QString>);
    void seekableChanged(bool);
    void hasVideoChanged(bool);

    void finished();
    void prefinishMarkReached(qint32);
    void aboutToFinish();
    void totalTimeChanged(qint64 length);
    void bufferStatus(int percentFilled);

    QMultiMap<QString, QString> metaData();
    void setMetaData(QMultiMap<QString, QString> newData);

private Q_SLOTS:
    void emitTick();

private:
    bool m_resumeState;
    State m_oldState;
    quint64 m_oldPos;
    quint64 currentPos;
    bool m_hasVideo;
    qint32 m_tickInterval;
    QTimer *m_tickTimer;
    Phonon::ErrorType m_error;
    QString m_errorString;
    qint64 m_totalTime;
    qint32 m_prefinishMark;
    qint32 m_transitionTime;
    MediaSource m_source;
    MediaSource m_nextSource;
    bool m_prefinishMarkReachedNotEmitted;
    bool m_aboutToFinishEmitted;
    int m_previousTickTime;

    State m_state;
    State m_pendingState;

    struct chunk
    {
        char        id[4];
        quint32     size;
    };

    struct RIFFHeader
    {
        chunk       descriptor;
        char        type[4];
    };

    struct WAVEHeader
    {
        chunk       descriptor;
        quint16     audioFormat;        // PCM = 1
        quint16     numChannels;
        quint32     sampleRate;
        quint32     byteRate;
        quint16     blockAlign;
        quint16     bitsPerSample;
        quint32     xFreq1;
        chunk       fact;
        quint32     xfact;
        chunk       data;
    };

    struct DATAHeader
    {
        chunk       descriptor;
        quint8      data[];
    };

    struct CombinedHeader
    {
        RIFFHeader  riff;
        WAVEHeader  wave;
        DATAHeader  data;
    };

    CombinedHeader      header;
};
}
} //namespace Phonon::Dummy

QT_END_NAMESPACE

#endif // PHONON_DUMMY_MEDIAOBJECT_H
