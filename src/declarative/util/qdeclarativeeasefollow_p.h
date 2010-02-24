/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEEASEFOLLOW_H
#define QDECLARATIVEEASEFOLLOW_H

#include <qdeclarative.h>
#include <qdeclarativepropertyvaluesource.h>

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeMetaProperty;
class QDeclarativeEaseFollowPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeEaseFollow : public QObject, 
                                           public QDeclarativePropertyValueSource
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeEaseFollow)
    Q_INTERFACES(QDeclarativePropertyValueSource)
    Q_ENUMS(ReversingMode)

    Q_PROPERTY(qreal source READ sourceValue WRITE setSourceValue NOTIFY sourceChanged)
    Q_PROPERTY(qreal velocity READ velocity WRITE setVelocity NOTIFY velocityChanged)
    Q_PROPERTY(qreal duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(ReversingMode reversingMode READ reversingMode WRITE setReversingMode NOTIFY reversingModeChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(qreal maximumEasingTime READ maximumEasingTime WRITE setMaximumEasingTime NOTIFY maximumEasingTimeChanged)

public:
    enum ReversingMode { Eased, Immediate, Sync };

    QDeclarativeEaseFollow(QObject *parent = 0);
    ~QDeclarativeEaseFollow();

    ReversingMode reversingMode() const;
    void setReversingMode(ReversingMode);

    qreal sourceValue() const;
    void setSourceValue(qreal);

    qreal velocity() const;
    void setVelocity(qreal);

    qreal duration() const;
    void setDuration(qreal);

    bool enabled() const;
    void setEnabled(bool enabled);

    qreal maximumEasingTime() const;
    void setMaximumEasingTime(qreal);

    virtual void setTarget(const QDeclarativeMetaProperty &);

Q_SIGNALS:
    void sourceChanged();
    void velocityChanged();
    void durationChanged();
    void reversingModeChanged();
    void enabledChanged();
    void maximumEasingTimeChanged();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeEaseFollow);

QT_END_HEADER

#endif // QDECLARATIVEEASEFOLLOW_H
