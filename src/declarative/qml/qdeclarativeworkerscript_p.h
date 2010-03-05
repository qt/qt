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

#ifndef QDECLARATIVEWORKERSCRIPT_P_H
#define QDECLARATIVEWORKERSCRIPT_P_H

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

#include "qdeclarative.h"
#include "qdeclarativeparserstatus.h"
#include <private/qlistmodelinterface_p.h>

#include <QtCore/qthread.h>
#include <QtScript/qscriptvalue.h>
#include <QtCore/qurl.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeWorkerScript;
class QDeclarativeWorkerScriptEnginePrivate;
class QDeclarativeWorkerScriptEngine : public QThread
{
Q_OBJECT
public:
    QDeclarativeWorkerScriptEngine(QDeclarativeEngine *parent = 0);
    virtual ~QDeclarativeWorkerScriptEngine();

    int registerWorkerScript(QDeclarativeWorkerScript *);
    void removeWorkerScript(int);
    void executeUrl(int, const QUrl &);
    void sendMessage(int, const QVariant &);

protected:
    virtual void run();

private:
    QDeclarativeWorkerScriptEnginePrivate *d;
};

class Q_DECLARATIVE_EXPORT QDeclarativeWorkerScript : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_INTERFACES(QDeclarativeParserStatus)
public:
    QDeclarativeWorkerScript(QObject *parent = 0);
    virtual ~QDeclarativeWorkerScript();

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
    QDeclarativeWorkerScriptEngine *m_engine;
    int m_scriptId;
    QUrl m_source;
};

class QDeclarativeWorkerListModelAgent;
class Q_DECLARATIVE_EXPORT QDeclarativeWorkerListModel : public QListModelInterface
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    QDeclarativeWorkerListModel(QObject * = 0);
    virtual ~QDeclarativeWorkerListModel();

    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void append(const QScriptValue &);
    Q_INVOKABLE void insert(int index, const QScriptValue&);
    Q_INVOKABLE QScriptValue get(int index) const;
    Q_INVOKABLE void set(int index, const QScriptValue &);
    Q_INVOKABLE void sync();

    QDeclarativeWorkerListModelAgent *agent();

    virtual QList<int> roles() const;
    virtual QString toString(int role) const;
    virtual int count() const;
    virtual QHash<int,QVariant> data(int index, const QList<int> &roles = (QList<int>())) const;
    virtual QVariant data(int index, int role) const;

Q_SIGNALS:
    void countChanged();

private:
    friend class QDeclarativeWorkerListModelAgent;

    QHash<int, QString> m_roles;
    QHash<QString, int> m_strings;
    QList<QHash<int, QVariant> > m_values;

    QDeclarativeWorkerListModelAgent *m_agent;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeWorkerScript);
QML_DECLARE_TYPE(QDeclarativeWorkerListModel);

QT_END_HEADER

#endif // QDECLARATIVEWORKERSCRIPT_P_H
