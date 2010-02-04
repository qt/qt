/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLTIMER_H
#define QMLTIMER_H

#include <qml.h>

#include <QtCore/qobject.h>
#include <QtCore/qabstractanimation.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlTimerPrivate;
class Q_DECLARATIVE_EXPORT QmlTimer : public QObject, public QmlParserStatus
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlTimer)
    Q_INTERFACES(QmlParserStatus)
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool repeat READ isRepeating WRITE setRepeating)
    Q_PROPERTY(bool triggeredOnStart READ triggeredOnStart WRITE setTriggeredOnStart)

public:
    QmlTimer(QObject *parent=0);

    void setInterval(int interval);
    int interval() const;

    bool isRunning() const;
    void setRunning(bool running);

    bool isRepeating() const;
    void setRepeating(bool repeating);

    bool triggeredOnStart() const;
    void setTriggeredOnStart(bool triggeredOnStart);

protected:
    void classBegin();
    void componentComplete();

public Q_SLOTS:
    void start();
    void stop();
    void restart();

Q_SIGNALS:
    void triggered();
    void runningChanged();

private:
    void update();

private Q_SLOTS:
    void ticked();
    void finished();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlTimer)

QT_END_HEADER

#endif
