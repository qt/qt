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

#ifndef QMLWORKERSCRIPT_P_H
#define QMLWORKERSCRIPT_P_H

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

#include "qml.h"
#include "qmlparserstatus.h"
#include <private/qlistmodelinterface_p.h>

#include <QtCore/qthread.h>
#include <QtScript/qscriptvalue.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

class QmlWorkerScript;
class QmlWorkerScriptEnginePrivate;
class QmlWorkerScriptEngine : public QThread
{
Q_OBJECT
public:
    QmlWorkerScriptEngine(QmlEngine *parent = 0);
    virtual ~QmlWorkerScriptEngine();

    int registerWorkerScript(QmlWorkerScript *);
    void removeWorkerScript(int);
    void executeUrl(int, const QUrl &);
    void sendMessage(int, const QVariant &);

protected:
    virtual void run();

private:
    QmlWorkerScriptEnginePrivate *d;
};

class QmlWorkerScript : public QObject, public QmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_INTERFACES(QmlParserStatus)
public:
    QmlWorkerScript(QObject *parent = 0);
    virtual ~QmlWorkerScript();

    QUrl source() const;
    void setSource(const QUrl &);

public slots:
    void sendMessage(const QScriptValue &);

signals:
    void sourceChanged();
    void message(const QScriptValue &messageObject);

protected:
    virtual void componentComplete();
    virtual bool event(QEvent *);

private:
    QmlWorkerScriptEngine *m_engine;
    int m_scriptId;
    QUrl m_source;
};

class QmlWorkerListModelAgent;
class QmlWorkerListModel : public QListModelInterface
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    QmlWorkerListModel(QObject * = 0);
    virtual ~QmlWorkerListModel();

    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void append(const QScriptValue &);
    Q_INVOKABLE void insert(int index, const QScriptValue&);
    Q_INVOKABLE QScriptValue get(int index) const;
    Q_INVOKABLE void set(int index, const QScriptValue &);

    QmlWorkerListModelAgent *agent();

    virtual QList<int> roles() const;
    virtual QString toString(int role) const;
    virtual int count() const;
    virtual QHash<int,QVariant> data(int index, const QList<int> &roles = (QList<int>())) const;
    virtual QVariant data(int index, int role) const;

Q_SIGNALS:
    void countChanged();

private:
    friend class QmlWorkerListModelAgent;

    QHash<int, QString> m_roles;
    QHash<QString, int> m_strings;
    QList<QHash<int, QVariant> > m_values;

    QmlWorkerListModelAgent *m_agent;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlWorkerScript);
QML_DECLARE_TYPE(QmlWorkerListModel);

#endif // QMLWORKERSCRIPT_P_H
