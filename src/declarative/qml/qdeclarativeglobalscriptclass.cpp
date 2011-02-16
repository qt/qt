/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativeglobalscriptclass_p.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qvector.h>
#include <QtScript/qscriptstring.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptvalueiterator.h>

#include <private/qscriptdeclarativeclass_p.h>

QT_BEGIN_NAMESPACE

/*
    Used to prevent any writes to the global object.
*/
QDeclarativeGlobalScriptClass::QDeclarativeGlobalScriptClass(QScriptEngine *engine)
: QScriptClass(engine)
{
    QString eval = QLatin1String("eval");
    QString version = QLatin1String("version");

    QScriptValue originalGlobalObject = engine->globalObject();

    QScriptValue newGlobalObject = engine->newObject();

    {
        QScriptValueIterator iter(originalGlobalObject);
        QVector<QString> names;
        QVector<QScriptValue> values;
        QVector<QScriptValue::PropertyFlags> flags;
        while (iter.hasNext()) {
            iter.next();

            QString name = iter.name();

            if (name == version)
                continue;

            if (name != eval) {
                names.append(name);
                values.append(iter.value());
                flags.append(iter.flags() | QScriptValue::Undeletable);
            }
            newGlobalObject.setProperty(iter.scriptName(), iter.value());

            m_illegalNames.insert(name);
        }
        m_staticGlobalObject = QScriptDeclarativeClass::newStaticScopeObject(
            engine, names.size(), names.constData(), values.constData(), flags.constData());
    }

    newGlobalObject.setScriptClass(this);
    engine->setGlobalObject(newGlobalObject);
}

QScriptClass::QueryFlags 
QDeclarativeGlobalScriptClass::queryProperty(const QScriptValue &object,
                                    const QScriptString &name,
                                    QueryFlags flags, uint *id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(flags);
    Q_UNUSED(id);
    return HandlesWriteAccess;
}

void QDeclarativeGlobalScriptClass::setProperty(QScriptValue &object, 
                                       const QScriptString &name,
                                       uint id, const QScriptValue &value)
{
    Q_UNUSED(object);
    Q_UNUSED(id);
    Q_UNUSED(value);
    QString error = QLatin1String("Invalid write to global property \"") + 
                    name.toString() + QLatin1Char('\"');
    engine()->currentContext()->throwError(error);
}

/* This method is for the use of tst_qdeclarativeecmascript::callQtInvokables() only */
void QDeclarativeGlobalScriptClass::explicitSetProperty(const QStringList &names, const QList<QScriptValue> &values)
{
    Q_ASSERT(names.count() == values.count());
    QScriptValue globalObject = engine()->globalObject();

    QScriptValue v = engine()->newObject();

    QScriptValueIterator iter(v);
    while (iter.hasNext()) {
        iter.next();
        v.setProperty(iter.scriptName(), iter.value());
    }

    for (int ii = 0; ii < names.count(); ++ii) {
        const QString &name = names.at(ii);
        const QScriptValue &value = values.at(ii);
        v.setProperty(name, value);
    }

    v.setScriptClass(this);

    engine()->setGlobalObject(v);
}

QT_END_NAMESPACE

