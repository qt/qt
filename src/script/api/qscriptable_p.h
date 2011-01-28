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

#ifndef QSCRIPTABLE_P_H
#define QSCRIPTABLE_P_H

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
#include "qscriptengine_p.h"
#include "qscriptcontext_p.h"
#include "qscriptable.h"

QT_BEGIN_NAMESPACE

class QScriptable;
class QScriptablePrivate
        : public QScriptLinkedNode
{
    Q_DECLARE_PUBLIC(QScriptable)
public:
    static inline QScriptablePrivate *get(QScriptable *q) { return q->d_func(); }

    QScriptablePrivate(const QScriptable* q) : q_ptr(const_cast<QScriptable*>(q)), m_engine(0) {}
    inline void reinitialize();

    inline QScriptEnginePrivate* engine() const;
    inline QScriptContextPrivate* context() const;
    inline QScriptPassPointer<QScriptValuePrivate> thisObject() const;
    inline int argumentCount() const;
    inline QScriptPassPointer<QScriptValuePrivate> argument(int index) const;

    inline QScriptEnginePrivate *swapEngine(QScriptEnginePrivate *);
private:
    QScriptable *q_ptr;
    QScriptEnginePrivate *m_engine;
};

QT_END_NAMESPACE

#endif
