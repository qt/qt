/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTDECLARATIVECLASS_P_H
#define QSCRIPTDECLARATIVECLASS_P_H

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

#include <QtCore/qobjectdefs.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptclass.h>

QT_BEGIN_NAMESPACE

class QScriptDeclarativeClassPrivate;
class PersistentIdentifierPrivate;
class Q_SCRIPT_EXPORT QScriptDeclarativeClass
{
public:
    typedef void* Identifier;
    typedef void* Object;

    static QScriptValue newObject(QScriptEngine *, QScriptDeclarativeClass *, Object);
    static QScriptDeclarativeClass *scriptClass(const QScriptValue &);
    static Object object(const QScriptValue &);

    static QScriptValue function(const QScriptValue &, const Identifier &);
    static QScriptValue property(const QScriptValue &, const Identifier &);

    class Q_SCRIPT_EXPORT PersistentIdentifier 
    {
    public:
        Identifier identifier;

        PersistentIdentifier();
        ~PersistentIdentifier();
        PersistentIdentifier(const PersistentIdentifier &other);
        PersistentIdentifier &operator=(const PersistentIdentifier &other);

    private:
        friend class QScriptDeclarativeClass;
        PersistentIdentifier(bool) : identifier(0), d(0) {}
        void *d;
    };

    QScriptDeclarativeClass(QScriptEngine *engine);
    virtual ~QScriptDeclarativeClass();

    QScriptEngine *engine() const;

    PersistentIdentifier createPersistentIdentifier(const QString &);
    PersistentIdentifier createPersistentIdentifier(const Identifier &);

    QString toString(const Identifier &);

    virtual QScriptClass::QueryFlags queryProperty(const Object &, const Identifier &, 
                                                   QScriptClass::QueryFlags flags);

    virtual QScriptValue property(const Object &, const Identifier &);
    virtual void setProperty(const Object &, const Identifier &name, const QScriptValue &);
    virtual QScriptValue::PropertyFlags propertyFlags(const Object &, const Identifier &);

    virtual QStringList propertyNames(const Object &);

    virtual QObject *toQObject(const Object &, bool *ok = 0);
    virtual QVariant toVariant(const Object &, bool *ok = 0);
    virtual void destroyed(const Object &);

protected:
    QScopedPointer<QScriptDeclarativeClassPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif
