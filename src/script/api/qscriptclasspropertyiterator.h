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

#ifndef QSCRIPTCLASSPROPERTYITERATOR_H
#define QSCRIPTCLASSPROPERTYITERATOR_H

#include <QtCore/qstring.h>

#ifndef QT_NO_SCRIPT

#include <QtScript/qscriptvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Script)

class QScriptClassPropertyIteratorPrivate;
class Q_SCRIPT_EXPORT QScriptClassPropertyIterator
{
protected:
    QScriptClassPropertyIterator(const QScriptValue &object);

public:
    virtual ~QScriptClassPropertyIterator();

    QScriptValue object() const;

    virtual bool hasNext() const = 0;
    virtual void next() = 0;

    virtual bool hasPrevious() const = 0;
    virtual void previous() = 0;

    virtual void toFront() = 0;
    virtual void toBack() = 0;

    virtual QScriptString name() const = 0;
    virtual uint id() const;
    virtual QScriptValue::PropertyFlags flags() const;

protected:
    QScriptClassPropertyIterator(const QScriptValue &object, QScriptClassPropertyIteratorPrivate &dd);
    QScriptClassPropertyIteratorPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QScriptClassPropertyIterator)
    Q_DISABLE_COPY(QScriptClassPropertyIterator)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SCRIPT

#endif
