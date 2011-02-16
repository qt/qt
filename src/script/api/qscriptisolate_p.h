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

#ifndef APIPREAMBLE_P_H
#define APIPREAMBLE_P_H

#include <v8.h>
#include <QtCore/qsharedpointer.h>
#include "qscriptengine_p.h"

QT_BEGIN_NAMESPACE

/**
  \internal
  Class used to switch to the right isolate. It does the same thing as v8::Isolate::Scope but
  it checks for a null engine.
  \attention We decided to put context switching "up" which means that it should be as high
  as possible on call stack. And it should be switched at most once per public API function call.
*/
class QScriptIsolate {
public:
    // OperationMode was introduced to reduce number of checking for a null engine pointer. If we
    // know that given pointer is not null than we should pass NotNullEngine as constructor argument
    // that would nicely remove checking on compilation time.
    enum OperationMode {Default, NotNullEngine};
    inline QScriptIsolate(const QScriptEnginePrivate *engine, const OperationMode mode = Default)
        : m_engine(engine)
        , m_mode(mode)
    {
        init();
    }

    inline ~QScriptIsolate()
    {
        if (m_mode == NotNullEngine || m_engine) {
            m_engine->exitIsolate();
        }
    }

private:
    inline void init() const
    {
        if (m_mode == NotNullEngine || m_engine) {
            Q_ASSERT(m_engine);
            m_engine->enterIsolate();
        }
    }

    Q_DISABLE_COPY(QScriptIsolate);
    const QScriptEnginePrivate *m_engine;
    const OperationMode m_mode;
};


QT_END_NAMESPACE

#endif // APIPREAMBLE_P_H
