/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QSOUNDEFFECT_H
#define QSOUNDEFFECT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <qml.h>

class QSoundEffectPrivate;
class QSoundEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int loopCount READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)

public:
    explicit QSoundEffect(QObject *parent = 0);
    ~QSoundEffect();

    QUrl source() const;
    void setSource(const QUrl &url);

    int loopCount() const;
    void setLoopCount(int loopCount);

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    int duration() const;

signals:
    void sourceChanged();
    void loopCountChanged();
    void volumeChanged();
    void mutedChanged();
    void durationChanged();

public slots:
    void play();
    void stop();

private slots:
    void repeat();

private:
    Q_DISABLE_COPY(QSoundEffect)

    int m_loopCount;
    int m_volume;
    bool m_muted;
    int m_runningCount;

    QSoundEffectPrivate* d;
};

QML_DECLARE_TYPE(QSoundEffect)

#endif // QSOUNDEFFECT_H
