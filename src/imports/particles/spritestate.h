/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
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

#ifndef SPRITESTATE_H
#define SPRITESTATE_H

#include <QObject>
#include <QUrl>
#include <QVariantMap>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class SpriteState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int frames READ frames WRITE setFrames NOTIFY framesChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(int durationVariance READ durationVariance WRITE setDurationVariance NOTIFY durationVarianceChanged)
    Q_PROPERTY(qreal speedModifiesDuration READ speedModifer WRITE setSpeedModifier NOTIFY speedModifierChanged)
    Q_PROPERTY(QVariantMap to READ to WRITE setTo NOTIFY toChanged)

public:
    explicit SpriteState(QObject *parent = 0);

    QUrl source() const
    {
        return m_source;
    }

    int frames() const
    {
        return m_frames;
    }

    int duration() const
    {
        return m_duration;
    }

    QString name() const
    {
        return m_name;
    }

    QVariantMap to() const
    {
        return m_to;
    }

    qreal speedModifer() const
    {
        return m_speedModifier;
    }

    int durationVariance() const
    {
        return m_durationVariance;
    }

signals:

    void sourceChanged(QUrl arg);

    void framesChanged(int arg);

    void durationChanged(int arg);

    void nameChanged(QString arg);

    void toChanged(QVariantMap arg);

    void speedModifierChanged(qreal arg);

    void durationVarianceChanged(int arg);

public slots:

    void setSource(QUrl arg)
    {
        if (m_source != arg) {
            m_source = arg;
            emit sourceChanged(arg);
        }
    }

    void setFrames(int arg)
    {
        if (m_frames != arg) {
            m_frames = arg;
            emit framesChanged(arg);
        }
    }

    void setDuration(int arg)
    {
        if (m_duration != arg) {
            m_duration = arg;
            emit durationChanged(arg);
        }
    }

    void setName(QString arg)
    {
        if (m_name != arg) {
            m_name = arg;
            emit nameChanged(arg);
        }
    }

    void setTo(QVariantMap arg)
    {
        if (m_to != arg) {
            m_to = arg;
            emit toChanged(arg);
        }
    }

    void setSpeedModifier(qreal arg)
    {
        if (m_speedModifier != arg) {
            m_speedModifier = arg;
            emit speedModifierChanged(arg);
        }
    }

    void setDurationVariance(int arg)
    {
        if (m_durationVariance != arg) {
            m_durationVariance = arg;
            emit durationVarianceChanged(arg);
        }
    }

private:
    friend class SpriteParticle;
    friend class SpriteEngine;
    QUrl m_source;
    int m_frames;
    int m_duration;
    QString m_name;
    QVariantMap m_to;
    qreal m_speedModifier;
    int m_durationVariance;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // SPRITESTATE_H
