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

#ifndef QMLCONNECTION_H
#define QMLCONNECTION_H

#include <qml.h>
#include <qmlscriptstring.h>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlBoundSignal;
class QmlContext;
class QmlConnectionPrivate;
class Q_DECLARATIVE_EXPORT QmlConnection : public QObject, public QmlParserStatus
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlConnection)

    Q_INTERFACES(QmlParserStatus)
    Q_PROPERTY(QObject *sender READ signalSender WRITE setSignalSender)
    Q_PROPERTY(QmlScriptString script READ script WRITE setScript)
    Q_PROPERTY(QString signal READ signal WRITE setSignal)

public:
    QmlConnection(QObject *parent=0);
    ~QmlConnection();

    QObject *signalSender() const;
    void setSignalSender(QObject *);
    QmlScriptString script() const;
    void setScript(const QmlScriptString&);
    QString signal() const;
    void setSignal(const QString&);

private:
    void disconnectIfValid();
    void connectIfValid();
    void componentComplete();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlConnection)

QT_END_HEADER

#endif
