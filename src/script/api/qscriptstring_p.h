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

#ifndef QSCRIPTSTRING_P_H
#define QSCRIPTSTRING_P_H

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

#include "qscriptshareddata_p.h"
#include "qscripttools_p.h"
#include "qscriptstring.h"
#include "v8.h"

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;

class QScriptStringPrivate
        : public QScriptSharedData
        , public QScriptLinkedNode
{
public:
    static inline QScriptString get(QScriptStringPrivate* d);
    static inline QScriptString get(QScriptPassPointer<QScriptStringPrivate> d);
    static inline QScriptStringPrivate* get(const QScriptString& p);

    inline QScriptStringPrivate();
    inline QScriptStringPrivate(QScriptEnginePrivate *, v8::Handle<v8::String>);
    inline ~QScriptStringPrivate();
    inline void reinitialize();

    inline bool operator==(const QScriptStringPrivate& other) const;
    inline bool operator!=(const QScriptStringPrivate& other) const;

    inline bool isValid() const;
    inline quint32 toArrayIndex(bool* ok = 0) const;
    inline QString toString() const;
    inline quint64 id() const;
    inline operator v8::Handle<v8::String>() const;
    inline v8::Handle<v8::String> asV8Value() const;
    inline QScriptEnginePrivate* engine() const;
private:
    Q_DISABLE_COPY(QScriptStringPrivate)
    QScriptEnginePrivate *m_engine;
    v8::Persistent<v8::String> m_string;
};

QT_END_NAMESPACE

#endif
