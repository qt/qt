/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTABLE_H
#define QSCRIPTABLE_H

#include <QtCore/qobjectdefs.h>

#ifndef QT_NO_SCRIPT

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Script)

#ifndef QT_NO_QOBJECT

class QScriptEngine;
class QScriptContext;
class QScriptValue;

class QScriptablePrivate;

class Q_SCRIPT_EXPORT QScriptable
{
public:
    QScriptable();
    ~QScriptable();

    QScriptEngine *engine() const;
    QScriptContext *context() const;
    QScriptValue thisObject() const;
    int argumentCount() const;
    QScriptValue argument(int index) const;

private:
    QScriptablePrivate *d_ptr;

    Q_DISABLE_COPY(QScriptable)
    Q_DECLARE_PRIVATE(QScriptable)
};

#endif // QT_NO_QOBJECT

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SCRIPT
#endif // QSCRIPTABLE_H
