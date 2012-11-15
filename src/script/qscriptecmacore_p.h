/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTECMACORE_P_H
#define QSCRIPTECMACORE_P_H

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

#include "qscriptfunction_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptvalueimplfwd_p.h"
#include "qscriptclassinfo_p.h"

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

class Core: public QScriptFunction
{
public:
    Core(QScriptEnginePrivate *engine, const QString &className,
         QScriptClassInfo::Type type);
    Core(QScriptEnginePrivate *engine, QScriptClassInfo *classInfo);
    virtual ~Core();

    inline QScriptEnginePrivate *engine() const
    { return m_engine; }

    inline QScriptClassInfo *classInfo() const
    { return m_classInfo; }

    void addPrototypeFunction(
        const QString &name, QScriptInternalFunctionSignature fun, int length,
        const QScriptValue::PropertyFlags flags = QScriptValue::SkipInEnumeration);
    void addConstructorFunction(
        const QString &name, QScriptInternalFunctionSignature fun, int length,
        const QScriptValue::PropertyFlags flags = QScriptValue::SkipInEnumeration);

    QString functionName() const;

    virtual void mark(QScriptEnginePrivate *eng, int generation);

public: // attributes
    QScriptValueImpl ctor;
    QScriptValueImpl publicPrototype;

protected:
    static QScriptValueImpl throwThisObjectTypeError(
        QScriptContextPrivate *context, const QString &functionName);

private:
    void addFunction(QScriptValueImpl &object, const QString &name,
                     QScriptInternalFunctionSignature fun, int length,
                     const QScriptValue::PropertyFlags flags);

    QScriptEnginePrivate *m_engine;
    QScriptClassInfo *m_classInfo;
};

} } // namespace QScript::Ecma

#endif // QT_NO_SCRIPT

QT_END_NAMESPACE

#endif
