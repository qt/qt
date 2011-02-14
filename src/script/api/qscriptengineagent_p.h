/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTENGINEAGENT_P_H
#define QSCRIPTENGINEAGENT_P_H

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
#include "qscripttools_p.h"

#include <v8.h>

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;
class QScriptEngineAgent;

class QScriptEngineAgentPrivate
        : public QScriptLinkedNode
{
    Q_DECLARE_PUBLIC(QScriptEngineAgent)
public:
    class UnloadData
            : public QScriptLinkedNode
    {
    public:
        inline UnloadData(QScriptEnginePrivate *engine, int64_t id);
        inline ~UnloadData();

        inline int64_t id() const;
        inline QScriptEnginePrivate *engine() const;

        static void UnloadHandler(v8::Persistent<v8::Value> object, void *dataPtr);
    private:
        QScriptEnginePrivate *m_engine;
        const int64_t m_scriptId;
    };

    inline static QScriptEngineAgent* get(QScriptEngineAgentPrivate *p);
    inline static QScriptEngineAgentPrivate* get(QScriptEngineAgent *p);

    inline QScriptEngineAgentPrivate(QScriptEngineAgent *q, QScriptEnginePrivate *engine);
    inline virtual ~QScriptEngineAgentPrivate();

    inline void scriptLoad(v8::Handle<v8::Script> script, const QString &program,
                            const QString &fileName, int baseLineNumber);
    inline void scriptUnload(int64_t id);

    inline void pushContext();
    inline void popContext();

    inline void attachTo(QScriptEnginePrivate *engine);
    inline QScriptEnginePrivate *engine() const;

    inline void kill();
private:
    inline QScriptEngineAgent *userCallback();

    QScriptEnginePrivate *m_engine;
    QScriptEngineAgent *q_ptr;
};

QT_END_NAMESPACE

#endif
