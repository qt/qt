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

#ifndef QSCRIPTVALUEITERATOR_H
#define QSCRIPTVALUEITERATOR_H

#include <QtScript/qscriptvalue.h>

#ifndef QT_NO_SCRIPT

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Script)

class QString;
class QScriptString;

class QScriptValueIteratorPrivate;
class Q_SCRIPT_EXPORT QScriptValueIterator
{
public:
    QScriptValueIterator(const QScriptValue &value);
    ~QScriptValueIterator();

    bool hasNext() const;
    void next();

    bool hasPrevious() const;
    void previous();

    QString name() const;
    QScriptString scriptName() const;

    QScriptValue value() const;
    void setValue(const QScriptValue &value);

    QScriptValue::PropertyFlags flags() const;

    void remove();

    void toFront();
    void toBack();

    QScriptValueIterator& operator=(QScriptValue &value);

private:
    QScriptValueIteratorPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QScriptValueIterator)
    Q_DISABLE_COPY(QScriptValueIterator)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SCRIPT

#endif // QSCRIPTVALUEITERATOR_H
