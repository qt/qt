/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLENGINE_H
#define QMLENGINE_H

#include <QtCore/qobject.h>
#include <QtCore/qmap.h>
#include <QtScript/qscriptvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlComponent;
class QmlEnginePrivate;
class QmlExpression;
class QmlContext;
class QUrl;
class QScriptEngine;
class QScriptContext;
class QNetworkAccessManager;
class Q_DECLARATIVE_EXPORT QmlEngine : public QObject
{
    Q_OBJECT
public:
    QmlEngine(QObject *p = 0);
    virtual ~QmlEngine();

    static QmlEngine *activeEngine();

    QmlContext *rootContext();
    QmlContext *activeContext();

    void clearComponentCache();

    void setNameSpacePaths(const QMap<QString,QString>& map);
    void addNameSpacePaths(const QMap<QString,QString>& map);
    void addNameSpacePath(const QString&,const QString&);
    QMap<QString,QString> nameSpacePaths() const;
    QUrl componentUrl(const QUrl& src, const QUrl& baseUrl) const;

    QList<QUrl> componentSearchPath(const QByteArray &qml, const QUrl &url) const;

    void setNetworkAccessManager(QNetworkAccessManager *);
    QNetworkAccessManager *networkAccessManager() const;

    static QmlContext *contextForObject(const QObject *);
    static void setContextForObject(QObject *, QmlContext *);

    static QScriptValue qmlScriptObject(QObject*, QmlEngine*);

    static QScriptValue createComponent(QScriptContext*, QScriptEngine*);
    static QScriptValue createQMLObject(QScriptContext*, QScriptEngine*);

private:
    // LK: move to the private class
    QScriptEngine *scriptEngine();
    friend class QFxItem; // XXX
    friend class QmlScriptPrivate; 
    friend class QmlCompositeTypeManager; 
    friend class QmlCompiler; 
    friend class QmlScriptClass;
    friend class QmlContext;
    friend class QmlContextPrivate;
    friend class QmlExpression;
    friend class QmlExpressionPrivate;
    friend class QmlBasicScript;
    friend class QmlVME;
    friend class QmlComponent;
    friend class QmlContextScriptClass; //###
    friend class QmlObjectScriptClass;      //###
    Q_DECLARE_PRIVATE(QmlEngine)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLENGINE_H
