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

#include "qscriptvalueiterator.h"

#ifndef QT_NO_SCRIPT

#include "qscriptstring.h"
#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptvalue_p.h"

#include "JSObject.h"
#include "PropertyNameArray.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptValueIterator

  \brief The QScriptValueIterator class provides a Java-style iterator for QScriptValue.

  \ingroup script
  \mainclass

  The QScriptValueIterator constructor takes a QScriptValue as
  argument.  After construction, the iterator is located at the very
  beginning of the sequence of properties. Here's how to iterate over
  all the properties of a QScriptValue:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 0

  The next() advances the iterator. The name(), value() and flags()
  functions return the name, value and flags of the last item that was
  jumped over.

  If you want to remove properties as you iterate over the
  QScriptValue, use remove(). If you want to modify the value of a
  property, use setValue().

  Note that QScriptValueIterator only iterates over the QScriptValue's
  own properties; i.e. it does not follow the prototype chain. You can
  use a loop like this to follow the prototype chain:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 1

  Note that QScriptValueIterator will not automatically skip over
  properties that have the QScriptValue::SkipInEnumeration flag set;
  that flag only affects iteration in script code.  If you want, you
  can skip over such properties with code like the following:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 2

  \sa QScriptValue::property()
*/

namespace QScript
{
JSC::UString qtStringToJSCUString(const QString &str);
QString qtStringFromJSCUString(const JSC::UString &str);
}

class QScriptValueIteratorPrivate
{
public:
    QScriptValueIteratorPrivate()
        : propertyNames(0), it(0)
    {}
    ~QScriptValueIteratorPrivate()
    {
        delete propertyNames;
    }
    void ensureInitialized()
    {
        if (propertyNames != 0)
            return;
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(object.engine());
        JSC::ExecState *exec = eng_p->globalObject->globalExec();
        propertyNames = new JSC::PropertyNameArray(exec);
        JSC::asObject(QScriptValuePrivate::get(object)->jscValue)->getPropertyNames(exec, *propertyNames);
    }

    QScriptValue object;
    JSC::PropertyNameArray *propertyNames;
    JSC::PropertyNameArray::const_iterator it;
};

/*!
  Constructs an iterator for traversing \a object. The iterator is
  set to be at the front of the sequence of properties (before the
  first property).
*/
QScriptValueIterator::QScriptValueIterator(const QScriptValue &object)
{
    if (!object.isObject()) {
        d_ptr = 0;
    } else {
        d_ptr = new QScriptValueIteratorPrivate();
        d_ptr->object = object;
    }
}

/*!
  Destroys the iterator.
*/
QScriptValueIterator::~QScriptValueIterator()
{
    if (d_ptr) {
        delete d_ptr;
        d_ptr = 0;
    }
}

/*!
  Returns true if there is at least one item ahead of the iterator
  (i.e. the iterator is \e not at the back of the property sequence);
  otherwise returns false.

  \sa next(), hasPrevious()
*/
bool QScriptValueIterator::hasNext() const
{
    Q_D(const QScriptValueIterator);
    if (!d)
        return false;
    const_cast<QScriptValueIteratorPrivate*>(d)->ensureInitialized();
    if (!d->it)
        return (d->propertyNames->size() != 0);
    return ((d->it+1) != d->propertyNames->end());
}

/*!
  Advances the iterator by one position.

  Calling this function on an iterator located at the back of the
  container leads to undefined results.

  \sa hasNext(), previous(), name()
*/
void QScriptValueIterator::next()
{
    Q_D(QScriptValueIterator);
    if (!d)
        return;
    d->ensureInitialized();
    if (!d->it)
        d->it = d->propertyNames->begin();
    else
        ++d->it;
}

/*!
  Returns true if there is at least one item behind the iterator
  (i.e. the iterator is \e not at the front of the property sequence);
  otherwise returns false.

  \sa previous(), hasNext()
*/
bool QScriptValueIterator::hasPrevious() const
{
    // ### implement me
    return false;
}

/*!
  Moves the iterator back by one position.

  Calling this function on an iterator located at the front of the
  container leads to undefined results.

  \sa hasPrevious(), next(), name()
*/
void QScriptValueIterator::previous()
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "not implemented");
}

/*!
  Moves the iterator to the front of the QScriptValue (before the
  first property).

  \sa toBack(), next()
*/
void QScriptValueIterator::toFront()
{
    Q_D(QScriptValueIterator);
    if (!d || !d->propertyNames)
        return;
    d->it = 0;
}

/*!
  Moves the iterator to the back of the QScriptValue (after the
  last property).

  \sa toFront(), previous()
*/
void QScriptValueIterator::toBack()
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "not implemented");
}

/*!
  Returns the name of the last property that was jumped over using
  next() or previous().

  \sa value(), flags()
*/
QString QScriptValueIterator::name() const
{
    Q_D(const QScriptValueIterator);
    if (!d || !d->it)
        return QString();
    return QScript::qtStringFromJSCUString(d->it->ustring());
}

/*!
  \since 4.4

  Returns the name of the last property that was jumped over using
  next() or previous().
*/
QScriptString QScriptValueIterator::scriptName() const
{
    Q_D(const QScriptValueIterator);
    if (!d || !d->it)
        return QScriptString();
    return d->object.engine()->toStringHandle(name());
}

/*!
  Returns the value of the last property that was jumped over using
  next() or previous().

  \sa setValue(), name()
*/
QScriptValue QScriptValueIterator::value() const
{
    Q_D(const QScriptValueIterator);
    if (!d || !d->it)
        return QScriptValue();
    return d->object.property(name());
}

/*!
  Sets the \a value of the last property that was jumped over using
  next() or previous().

  \sa value(), name()
*/
void QScriptValueIterator::setValue(const QScriptValue &value)
{
    Q_D(QScriptValueIterator);
    if (!d || !d->it)
        return;
    d->object.setProperty(name(), value);
}

/*!
  Returns the flags of the last property that was jumped over using
  next() or previous().

  \sa value()
*/
QScriptValue::PropertyFlags QScriptValueIterator::flags() const
{
    Q_D(const QScriptValueIterator);
    if (!d || !d->it)
        return 0;
    return d->object.propertyFlags(name());
}

/*!
  Removes the last property that was jumped over using next()
  or previous().

  \sa setValue()
*/
void QScriptValueIterator::remove()
{
    Q_D(QScriptValueIterator);
    if (!d || !d->it)
        return;
    d->object.setProperty(name(), QScriptValue());
}

/*!
  Makes the iterator operate on \a object. The iterator is set to be
  at the front of the sequence of properties (before the first
  property).
*/
QScriptValueIterator& QScriptValueIterator::operator=(QScriptValue &object)
{
    if (d_ptr) {
        delete d_ptr;
        d_ptr = 0;
    }
    if (object.isObject()) {
        d_ptr = new QScriptValueIteratorPrivate();
        d_ptr->object = object;
    }
    return *this;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
