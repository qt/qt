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

#ifndef QSCRIPTSTRING_H
#define QSCRIPTSTRING_H

#include <QtCore/qstring.h>

#ifndef QT_NO_SCRIPT

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Script)

class QScriptEngine;
class QScriptStringPrivate;

class Q_SCRIPT_EXPORT QScriptString
{
public:
    QScriptString();
    QScriptString(const QScriptString &other);
    ~QScriptString();

    QScriptString &operator=(const QScriptString &other);

    bool isValid() const;

    bool operator==(const QScriptString &other) const;
    bool operator!=(const QScriptString &other) const;

    QString toString() const;
    operator QString() const;

private:
    QScriptStringPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QScriptString)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SCRIPT
#endif // QSCRIPTSTRING_H
