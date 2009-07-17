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

#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <QtCore/qmap.h>
#include <QtScript/qscriptvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlComponent;
class QmlEnginePrivate;
class QmlImportsPrivate;
class QmlExpression;
class QmlContext;
class QmlType;
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

    QmlContext *rootContext();

    void clearComponentCache();

    struct Imports {
        Imports();
        ~Imports();
        void setBaseUrl(const QUrl& url);
        QUrl baseUrl() const { return base; }
    private:
        friend class QmlEngine;
        QUrl base;
        QmlImportsPrivate *d;
    };
    struct ImportedNamespace;

    void addImportPath(const QString& dir);
    enum ImportType { LibraryImport, FileImport };
    bool addToImport(Imports*, const QString& uri, const QString& prefix, const QString& version, ImportType type) const;
    bool resolveType(const Imports&, const QByteArray& type, QmlType** type_return, QUrl* url_return, ImportedNamespace** ns_return=0) const;
    void resolveTypeInNamespace(ImportedNamespace*, const QByteArray& type, QmlType** type_return, QUrl* url_return ) const;

    void setNetworkAccessManager(QNetworkAccessManager *);
    QNetworkAccessManager *networkAccessManager() const;

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &);

    static QmlContext *contextForObject(const QObject *);
    static void setContextForObject(QObject *, QmlContext *);

    static QScriptValue qmlScriptObject(QObject*, QmlEngine*);

    static QScriptValue createComponent(QScriptContext*, QScriptEngine*);
    static QScriptValue createQmlObject(QScriptContext*, QScriptEngine*);

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
