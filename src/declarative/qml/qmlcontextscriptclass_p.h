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

#ifndef QMLCONTEXTSCRIPTCLASS_P_H
#define QMLCONTEXTSCRIPTCLASS_P_H

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

#include "qmltypenamecache_p.h"
#include "qmlscriptclass_p.h"

QT_BEGIN_NAMESPACE

class QmlEngine;
class QmlContext;
class QmlContextScriptClass : public QmlScriptClass
{
public:
    QmlContextScriptClass(QmlEngine *);
    ~QmlContextScriptClass();

    QScriptValue newContext(QmlContext *, QObject * = 0);
    QScriptValue newSharedContext();

    QmlContext *contextFromValue(const QScriptValue &);

protected:
    virtual QScriptClass::QueryFlags queryProperty(Object *, const Identifier &, 
                                                   QScriptClass::QueryFlags flags);
    virtual ScriptValue property(Object *, const Identifier &);
    virtual void setProperty(Object *, const Identifier &name, const QScriptValue &);

private:
    QScriptClass::QueryFlags queryProperty(QmlContext *, QObject *scopeObject, 
                                           const Identifier &,
                                           QScriptClass::QueryFlags flags,
                                           bool includeTypes);

    QmlEngine *engine;

    QObject *lastScopeObject;
    QmlContext *lastContext;
    QmlTypeNameCache::Data *lastData;
    int lastPropertyIndex;
    int lastDefaultObject;
    QScriptValue lastFunction;

    uint m_id;
};

QT_END_NAMESPACE

#endif // QMLCONTEXTSCRIPTCLASS_P_H

