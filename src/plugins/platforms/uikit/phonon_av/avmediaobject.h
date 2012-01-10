/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef AV_MEDIAOBJECT_H
#define AV_MEDIAOBJECT_H

#include <phonon/mediaobjectinterface.h>

#include <QtCore/QMultiMap>

class AVAudioOutput;
class AVMediaObjectPrivate;

using namespace Phonon;

class AVMediaObject : public QObject, public Phonon::MediaObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(Phonon::MediaObjectInterface)

public:
    AVMediaObject(QObject *parent);
    ~AVMediaObject();
    void play();
    void pause();
    void stop();
    void seek(qint64 milliseconds);
    qint32 tickInterval() const;
    void setTickInterval(qint32 newTickInterval);
    bool hasVideo() const;
    bool isSeekable() const;
    qint64 currentTime() const;
    Phonon::State state() const;
    QString errorString() const;
    Phonon::ErrorType errorType() const;
    qint64 totalTime() const;
    Phonon::MediaSource source() const;
    void setSource(const Phonon::MediaSource &source);
    void setNextSource(const Phonon::MediaSource &source);

    qint32 prefinishMark() const;
    void setPrefinishMark(qint32 newPrefinishMark);
    qint32 transitionTime() const;
    void setTransitionTime(qint32);

Q_SIGNALS:
    void stateChanged(Phonon::State newstate, Phonon::State oldstate);
    void totalTimeChanged(qint64 length);
    void currentSourceChanged(const MediaSource&);
    void aboutToFinish();
    void finished();
    //TODO
    void tick(qint64 time);

// unused
    void seekableChanged(bool);
    void hasVideoChanged(bool);
    void bufferStatus(int);
    void prefinishMarkReached(qint32);
    void metaDataChanged(const QMultiMap<QString, QString> &);

public:
    void setAudioOutput(AVAudioOutput *audioOutput);
    void handlePlayerFinished();

private Q_SLOTS:
    void setVolume(qreal newVolume);

private:
    void changeState(Phonon::State state);
    bool checkPlayer() const;

    qint32 m_tickInterval;
    Phonon::State m_state;
    mutable QString m_errorString;
    mutable Phonon::ErrorType m_errorType;
    Phonon::MediaSource m_mediaSource;

    AVAudioOutput *m_output;
    AVMediaObjectPrivate *d;
};

QT_END_NAMESPACE

#endif // PHONON_MEDIAOBJECT_H
