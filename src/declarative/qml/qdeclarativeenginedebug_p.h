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

#ifndef QDECLARATIVEENGINEDEBUG_P_H
#define QDECLARATIVEENGINEDEBUG_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qdeclarativedebugservice_p.h>

#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QDeclarativeEngine;
class QDeclarativeContext;
class QDeclarativeWatcher;
class QDataStream;

class QDeclarativeEngineDebugServer : public QDeclarativeDebugService
{
    Q_OBJECT
public:
    QDeclarativeEngineDebugServer(QObject * = 0);

    struct QDeclarativeObjectData {
        QUrl url;
        int lineNumber;
        int columnNumber;
        QString idString;
        QString objectName;
        QString objectType;
        int objectId;
        int contextId;
    };

    struct QDeclarativeObjectProperty {
        enum Type { Unknown, Basic, Object, List, SignalProperty };
        Type type;
        QString name;
        QVariant value;
        QString valueTypeName;
        QString binding;
        bool hasNotifySignal;
    };

    static void addEngine(QDeclarativeEngine *);
    static void remEngine(QDeclarativeEngine *);

protected:
    virtual void messageReceived(const QByteArray &);

private Q_SLOTS:
    void propertyChanged(int id, int objectId, const QMetaProperty &property, const QVariant &value);

private:
    void buildObjectList(QDataStream &, QDeclarativeContext *);
    void buildObjectDump(QDataStream &, QObject *, bool);
    QDeclarativeObjectData objectData(QObject *);
    QDeclarativeObjectProperty propertyData(QObject *, int);
    QVariant valueContents(const QVariant &defaultValue) const;
    void setBinding(int objectId, const QString &propertyName, const QVariant &expression, bool isLiteralValue);
    void resetBinding(int objectId, const QString &propertyName);
    void setMethodBody(int objectId, const QString &method, const QString &body);

    static QList<QDeclarativeEngine *> m_engines;
    QDeclarativeWatcher *m_watch;
};
Q_DECLARATIVE_EXPORT QDataStream &operator<<(QDataStream &, const QDeclarativeEngineDebugServer::QDeclarativeObjectData &);
Q_DECLARATIVE_EXPORT QDataStream &operator>>(QDataStream &, QDeclarativeEngineDebugServer::QDeclarativeObjectData &);
Q_DECLARATIVE_EXPORT QDataStream &operator<<(QDataStream &, const QDeclarativeEngineDebugServer::QDeclarativeObjectProperty &);
Q_DECLARATIVE_EXPORT QDataStream &operator>>(QDataStream &, QDeclarativeEngineDebugServer::QDeclarativeObjectProperty &);

QT_END_NAMESPACE

#endif // QDECLARATIVEENGINEDEBUG_P_H

