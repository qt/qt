/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTFUNCTION_P_H
#define QSCRIPTFUNCTIOn_P_H

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

#include <QtCore/qglobal.h>

#ifndef QT_NO_SCRIPT

#include "qscriptengine.h"

#include "PrototypeFunction.h"

QT_BEGIN_NAMESPACE

namespace QScript
{

class FunctionWrapper : public JSC::PrototypeFunction // ### subclass InternalFunction instead
{
public:
    // work around CELL_SIZE limitation
    struct Data
    {
        QScriptEngine::FunctionSignature function;
    };

    FunctionWrapper(JSC::ExecState*, int length, const JSC::Identifier&,
                    QScriptEngine::FunctionSignature);
    ~FunctionWrapper();

private:
    virtual JSC::ConstructType getConstructData(JSC::ConstructData&);

    static JSC::JSValue JSC_HOST_CALL proxyCall(JSC::ExecState *, JSC::JSObject *,
                                                JSC::JSValue, const JSC::ArgList &);
    static JSC::JSObject* proxyConstruct(JSC::ExecState *, JSC::JSObject *,
                                         const JSC::ArgList &);

private:
    Data *data;
};

class FunctionWithArgWrapper : public JSC::PrototypeFunction
{
public:
    // work around CELL_SIZE limitation
    struct Data
    {
        QScriptEngine::FunctionWithArgSignature function;
        void *arg;
    };

    FunctionWithArgWrapper(JSC::ExecState*, int length, const JSC::Identifier&,
                           QScriptEngine::FunctionWithArgSignature, void *);
    ~FunctionWithArgWrapper();

private:
    virtual JSC::ConstructType getConstructData(JSC::ConstructData&);

    static JSC::JSValue JSC_HOST_CALL proxyCall(JSC::ExecState *, JSC::JSObject *,
                                                JSC::JSValue , const JSC::ArgList &);
    static JSC::JSObject* proxyConstruct(JSC::ExecState *, JSC::JSObject *,
                                         const JSC::ArgList &);

private:
    Data *data;
};

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
