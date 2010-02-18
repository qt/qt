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
#ifndef QMLDEBUG_H
#define QMLDEBUG_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlDebugConnection;
class QmlDebugWatch;
class QmlDebugPropertyWatch;
class QmlDebugObjectExpressionWatch;
class QmlDebugEnginesQuery;
class QmlDebugRootContextQuery;
class QmlDebugObjectQuery;
class QmlDebugExpressionQuery;
class QmlDebugPropertyReference;
class QmlDebugContextReference;
class QmlDebugObjectReference;
class QmlDebugFileReference;
class QmlDebugEngineReference;
class QmlEngineDebugPrivate;
class Q_DECLARATIVE_EXPORT QmlEngineDebug : public QObject
{
Q_OBJECT
public:
    QmlEngineDebug(QmlDebugConnection *, QObject * = 0);

    QmlDebugPropertyWatch *addWatch(const QmlDebugPropertyReference &,
                            QObject *parent = 0);
    QmlDebugWatch *addWatch(const QmlDebugContextReference &, const QString &,
                            QObject *parent = 0);
    QmlDebugObjectExpressionWatch *addWatch(const QmlDebugObjectReference &, const QString &,
                            QObject *parent = 0);
    QmlDebugWatch *addWatch(const QmlDebugObjectReference &,
                            QObject *parent = 0);
    QmlDebugWatch *addWatch(const QmlDebugFileReference &,
                            QObject *parent = 0);

    void removeWatch(QmlDebugWatch *watch);

    QmlDebugEnginesQuery *queryAvailableEngines(QObject *parent = 0);
    QmlDebugRootContextQuery *queryRootContexts(const QmlDebugEngineReference &,
                                                QObject *parent = 0);
    QmlDebugObjectQuery *queryObject(const QmlDebugObjectReference &, 
                                     QObject *parent = 0);
    QmlDebugObjectQuery *queryObjectRecursive(const QmlDebugObjectReference &, 
                                              QObject *parent = 0);
    QmlDebugExpressionQuery *queryExpressionResult(int objectDebugId, 
                                                   const QString &expr,
                                                   QObject *parent = 0);

private:
    Q_DECLARE_PRIVATE(QmlEngineDebug)
};

class Q_DECLARATIVE_EXPORT QmlDebugWatch : public QObject
{
Q_OBJECT
public:
    enum State { Waiting, Active, Inactive, Dead };

    QmlDebugWatch(QObject *);
    ~QmlDebugWatch();

    int queryId() const;
    int objectDebugId() const;
    State state() const;

Q_SIGNALS:
    void stateChanged(QmlDebugWatch::State);
    //void objectChanged(int, const QmlDebugObjectReference &);
    //void valueChanged(int, const QVariant &);

    // Server sends value as string if it is a user-type variant
    void valueChanged(const QByteArray &name, const QVariant &value);

private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    void setState(State);
    State m_state;
    int m_queryId;
    QmlEngineDebug *m_client;
    int m_objectDebugId;
};

class Q_DECLARATIVE_EXPORT QmlDebugPropertyWatch : public QmlDebugWatch
{
    Q_OBJECT
public:
    QmlDebugPropertyWatch(QObject *parent);

    QString name() const;

private:
    friend class QmlEngineDebug;
    QString m_name;
};

class Q_DECLARATIVE_EXPORT QmlDebugObjectExpressionWatch : public QmlDebugWatch
{
    Q_OBJECT
public:
    QmlDebugObjectExpressionWatch(QObject *parent);

    QString expression() const;

private:
    friend class QmlEngineDebug;
    QString m_expr;
    int m_debugId;
};


class Q_DECLARATIVE_EXPORT QmlDebugQuery : public QObject
{
Q_OBJECT
public:
    enum State { Waiting, Error, Completed };

    State state() const;
    bool isWaiting() const;

//    bool waitUntilCompleted();

Q_SIGNALS:
    void stateChanged(QmlDebugQuery::State);

protected:
    QmlDebugQuery(QObject *);

private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    void setState(State);
    State m_state;
};

class Q_DECLARATIVE_EXPORT QmlDebugFileReference 
{
public:
    QmlDebugFileReference();
    QmlDebugFileReference(const QmlDebugFileReference &);
    QmlDebugFileReference &operator=(const QmlDebugFileReference &);

    QUrl url() const;
    void setUrl(const QUrl &);
    int lineNumber() const;
    void setLineNumber(int);
    int columnNumber() const;
    void setColumnNumber(int);

private:
    friend class QmlEngineDebugPrivate;
    QUrl m_url;
    int m_lineNumber;
    int m_columnNumber;
};

class Q_DECLARATIVE_EXPORT QmlDebugEngineReference
{
public:
    QmlDebugEngineReference();
    QmlDebugEngineReference(int);
    QmlDebugEngineReference(const QmlDebugEngineReference &);
    QmlDebugEngineReference &operator=(const QmlDebugEngineReference &);

    int debugId() const;
    QString name() const;

private:
    friend class QmlEngineDebugPrivate;
    int m_debugId;
    QString m_name;
};

class Q_DECLARATIVE_EXPORT QmlDebugObjectReference
{
public:
    QmlDebugObjectReference();
    QmlDebugObjectReference(int);
    QmlDebugObjectReference(const QmlDebugObjectReference &);
    QmlDebugObjectReference &operator=(const QmlDebugObjectReference &);

    int debugId() const;
    QString className() const;
    QString name() const;

    QmlDebugFileReference source() const;
    int contextDebugId() const;

    QList<QmlDebugPropertyReference> properties() const;
    QList<QmlDebugObjectReference> children() const;

private:
    friend class QmlEngineDebugPrivate;
    int m_debugId;
    QString m_class;
    QString m_name;
    QmlDebugFileReference m_source;
    int m_contextDebugId;
    QList<QmlDebugPropertyReference> m_properties;
    QList<QmlDebugObjectReference> m_children;
};

class Q_DECLARATIVE_EXPORT QmlDebugContextReference
{
public:
    QmlDebugContextReference();
    QmlDebugContextReference(const QmlDebugContextReference &);
    QmlDebugContextReference &operator=(const QmlDebugContextReference &);

    int debugId() const;
    QString name() const;

    QList<QmlDebugObjectReference> objects() const;
    QList<QmlDebugContextReference> contexts() const;

private:
    friend class QmlEngineDebugPrivate;
    int m_debugId;
    QString m_name;
    QList<QmlDebugObjectReference> m_objects;
    QList<QmlDebugContextReference> m_contexts;
};

class Q_DECLARATIVE_EXPORT QmlDebugPropertyReference
{
public:
    QmlDebugPropertyReference();
    QmlDebugPropertyReference(const QmlDebugPropertyReference &);
    QmlDebugPropertyReference &operator=(const QmlDebugPropertyReference &);

    int objectDebugId() const;
    QString name() const;
    QVariant value() const;
    QString valueTypeName() const;
    QString binding() const;
    bool hasNotifySignal() const;

private:
    friend class QmlEngineDebugPrivate;
    int m_objectDebugId;
    QString m_name;
    QVariant m_value;
    QString m_valueTypeName;
    QString m_binding;
    bool m_hasNotifySignal;
};


class Q_DECLARATIVE_EXPORT QmlDebugEnginesQuery : public QmlDebugQuery
{
Q_OBJECT
public:
    virtual ~QmlDebugEnginesQuery();
    QList<QmlDebugEngineReference> engines() const;
private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    QmlDebugEnginesQuery(QObject *);
    QmlEngineDebug *m_client;
    int m_queryId;
    QList<QmlDebugEngineReference> m_engines;
};

class Q_DECLARATIVE_EXPORT QmlDebugRootContextQuery : public QmlDebugQuery
{
Q_OBJECT
public:
    virtual ~QmlDebugRootContextQuery();
    QmlDebugContextReference rootContext() const;
private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    QmlDebugRootContextQuery(QObject *);
    QmlEngineDebug *m_client;
    int m_queryId;
    QmlDebugContextReference m_context;
};

class Q_DECLARATIVE_EXPORT QmlDebugObjectQuery : public QmlDebugQuery
{
Q_OBJECT
public:
    virtual ~QmlDebugObjectQuery();
    QmlDebugObjectReference object() const;
private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    QmlDebugObjectQuery(QObject *);
    QmlEngineDebug *m_client;
    int m_queryId;
    QmlDebugObjectReference m_object;

};

class Q_DECLARATIVE_EXPORT QmlDebugExpressionQuery : public QmlDebugQuery
{
Q_OBJECT
public:
    virtual ~QmlDebugExpressionQuery();
    QString expression() const;
    QVariant result() const;
private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    QmlDebugExpressionQuery(QObject *);
    QmlEngineDebug *m_client;
    int m_queryId;
    QString m_expr;
    QVariant m_result;

};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLDEBUG_H
