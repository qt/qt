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

#include "qscriptvalueiterator.h"

#include "qscriptisolate_p.h"
#include "qscriptstring_p.h"
#include "qscriptvalue_p.h"
#include "qscriptclass_p.h"
#include "qscriptclasspropertyiterator.h"
#include "qscriptengine_p.h"
#include "qscript_impl_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QScriptValueIterator

    \brief The QScriptValueIterator class provides a Java-style iterator for QScriptValue.

    \ingroup script


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

using v8::Persistent;
using v8::Local;
using v8::Array;
using v8::String;
using v8::Handle;
using v8::Object;
using v8::Value;

// FIXME (Qt5) This class should be refactored. It should use the common Iterator interface.
// FIXME it could be faster!
class QScriptValueIteratorPrivate {
public:
    inline QScriptValueIteratorPrivate(const QScriptValuePrivate* value);
    inline ~QScriptValueIteratorPrivate();

    inline bool hasNext();
    inline void next();

    inline bool hasPrevious();
    inline void previous();

    inline QString name() const;
    inline QScriptPassPointer<QScriptStringPrivate> scriptName() const;

    inline QScriptPassPointer<QScriptValuePrivate> value() const;
    inline void setValue(const QScriptValuePrivate* value);

    inline void remove();

    inline void toFront();
    inline void toBack();

    QScriptValue::PropertyFlags flags() const;

    inline bool isValid() const;
    inline QScriptEnginePrivate* engine() const;
private:
    Q_DISABLE_COPY(QScriptValueIteratorPrivate)
    //void dump(QString) const;

    QScriptSharedDataPointer<QScriptValuePrivate> m_object;
    QList<QScriptSharedDataPointer<QScriptStringPrivate> > m_names;
    QMutableListIterator<QScriptSharedDataPointer<QScriptStringPrivate> > m_iterator;
    QScriptClassPropertyIterator *m_classIterator;
    bool m_usingClassIterator;
};

inline QScriptValueIteratorPrivate::QScriptValueIteratorPrivate(const QScriptValuePrivate* value)
    : m_object(const_cast<QScriptValuePrivate*>(value))
    , m_iterator(m_names)
    , m_classIterator(0)
    , m_usingClassIterator(false)
{
    Q_ASSERT(value);
    QScriptEnginePrivate *engine = m_object->engine();
    QScriptIsolate api(engine);
    if (!m_object->isObject())
        m_object = 0;
    else {
        v8::HandleScope scope;
        Handle<Value> tmp = *value;
        Handle<Object> obj = Handle<Object>::Cast(tmp);
        Local<Array> names;

        // check if the value is a script class instance
        QScriptClassObject *data = QScriptClassObject::safeGet(value);
        if (data
            && data->scriptClass()
            && (m_classIterator = data->scriptClass()->userCallback()->newIterator(QScriptValuePrivate::get(value)))) {
            // we need to wrap custom iterator.
            names = engine->getOwnPropertyNames(data->original());
        } else
            names = engine->getOwnPropertyNames(obj);

        uint32_t count = names->Length();
        Local<String> name;
        m_names.reserve(count); // The count is the maximal count of values.
        for (uint32_t i = count - 1; i < count; --i) {
            name = names->Get(i)->ToString();
            m_names.append(QScriptSharedDataPointer<QScriptStringPrivate>(new QScriptStringPrivate(engine, name)));
        }

        // Reinitialize the iterator.
        m_iterator = m_names;
    }
}

inline QScriptValueIteratorPrivate::~QScriptValueIteratorPrivate()
{
    delete m_classIterator;
}

inline bool QScriptValueIteratorPrivate::hasNext()
{
    //dump("hasNext()");
    return isValid()
            ? m_iterator.hasNext() || (m_classIterator && m_classIterator->hasNext())
            : false;
}

inline void QScriptValueIteratorPrivate::next()
{
    // FIXME (Qt5) This method should return a value (QTBUG-11226).
    //dump("next();");
    if (m_iterator.hasNext())
        m_iterator.next();
    else if (m_classIterator) {
        m_usingClassIterator = true;
        m_classIterator->next();
    }
}

inline bool QScriptValueIteratorPrivate::hasPrevious()
{
    //dump("hasPrevious()");
    return isValid()
            ? (m_classIterator && m_classIterator->hasPrevious()) || m_iterator.hasPrevious()
            : false;
}

inline void QScriptValueIteratorPrivate::previous()
{
    // FIXME (Qt5) This method should return a value (QTBUG-11226).
    //dump("previous();");
    if (m_classIterator && m_classIterator->hasPrevious())
        m_classIterator->previous();
    else {
        m_usingClassIterator = false;
        m_iterator.previous();
    }
}

inline QString QScriptValueIteratorPrivate::name() const
{
    //dump("name");
    if (!isValid())
        return QString();

    if (m_usingClassIterator)
        return m_classIterator->name().toString();
    return m_iterator.value()->toString();
}

inline QScriptPassPointer<QScriptStringPrivate> QScriptValueIteratorPrivate::scriptName() const
{
    //dump("scriptName");
    if (!isValid())
        return new QScriptStringPrivate();

    if (!m_usingClassIterator)
        return m_iterator.value().data();
    return QScriptStringPrivate::get(m_classIterator->name());
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValueIteratorPrivate::value() const
{
    //dump("value()");
    if (!isValid())
        return InvalidValue();
    // FIXME it could be faster!
    if (m_usingClassIterator)
        return m_object->property(m_classIterator->name().toString(), QScriptValue::ResolveLocal);
    return m_object->property(m_iterator.value().data(), QScriptValue::ResolveLocal);
}

inline void QScriptValueIteratorPrivate::setValue(const QScriptValuePrivate* value)
{
    if (!isValid())
        return;
    if (m_usingClassIterator)
        m_object->setProperty(m_classIterator->name(), const_cast<QScriptValuePrivate*>(value));
    m_object->setProperty(m_iterator.value().data(), const_cast<QScriptValuePrivate*>(value));
}

inline void QScriptValueIteratorPrivate::remove()
{
    //dump("remove();");
    if (!isValid())
        return;
    if (m_usingClassIterator) {
        m_object->deleteProperty(m_classIterator->name());
        return;
    }
    if (m_object->deleteProperty(m_iterator.value()->toString()))
        m_iterator.remove();
}

inline void QScriptValueIteratorPrivate::toFront()
{
    //dump("toFront();");
    m_iterator.toFront();
    if (m_classIterator)
        m_classIterator->toFront();
}

inline void QScriptValueIteratorPrivate::toBack()
{
    //dump("toBack();");
    m_iterator.toBack();
    if (m_classIterator)
        m_classIterator->toBack();
}

QScriptValue::PropertyFlags QScriptValueIteratorPrivate::flags() const
{
    if (!isValid())
        return QScriptValue::PropertyFlags(0);

    v8::HandleScope scope;
    if (m_usingClassIterator)
        return m_object->propertyFlags(QScriptConverter::toString(m_classIterator->name()), QScriptValue::ResolveLocal);
    return m_object->propertyFlags(m_iterator.value().data(), QScriptValue::ResolveLocal);
}

inline bool QScriptValueIteratorPrivate::isValid() const
{
    bool result = m_object ? m_object->isValid() : false;
    // We know that if this object is still valid then it is an object
    // if this assumption is not correct then some other logic in this class
    // have to be changed too.
    Q_ASSERT(!result || m_object->isObject());
    return result;
}

inline QScriptEnginePrivate* QScriptValueIteratorPrivate::engine() const
{
    return m_object ? m_object->engine() : 0;
}

//void QScriptValueIteratorPrivate::dump(QString fname) const
//{
//    qDebug() << "    *** " << fname << " ***";
//    foreach(Persistent<String> name, m_names) {
//        qDebug() << "        - " << QScriptConverter::toString(name);
//    }
//}

/*!
    Constructs an iterator for traversing \a object. The iterator is
    set to be at the front of the sequence of properties (before the
    first property).
*/
QScriptValueIterator::QScriptValueIterator(const QScriptValue& object)
    : d_ptr(new QScriptValueIteratorPrivate(QScriptValuePrivate::get(object)))
{}

/*!
    Destroys the iterator.
*/
QScriptValueIterator::~QScriptValueIterator()
{}

/*!
    Returns true if there is at least one item ahead of the iterator
    (i.e. the iterator is \e not at the back of the property sequence);
    otherwise returns false.

    \sa next(), hasPrevious()
*/
bool QScriptValueIterator::hasNext() const
{
    return d_ptr->hasNext();
}

/*!
    Advances the iterator by one position.

    Calling this function on an iterator located at the back of the
    container leads to undefined results.

    \sa hasNext(), previous(), name()
*/
void QScriptValueIterator::next()
{
    d_ptr->next();
}

/*!
    Returns true if there is at least one item behind the iterator
    (i.e. the iterator is \e not at the front of the property sequence);
    otherwise returns false.

    \sa previous(), hasNext()
*/
bool QScriptValueIterator::hasPrevious() const
{
    return d_ptr->hasPrevious();
}

/*!
    Moves the iterator back by one position.

    Calling this function on an iterator located at the front of the
    container leads to undefined results.

    \sa hasPrevious(), next(), name()
*/
void QScriptValueIterator::previous()
{
    d_ptr->previous();
}

/*!
    Moves the iterator to the front of the QScriptValue (before the
    first property).

    \sa toBack(), next()
*/
void QScriptValueIterator::toFront()
{
    d_ptr->toFront();
}

/*!
    Moves the iterator to the back of the QScriptValue (after the
    last property).

    \sa toFront(), previous()
*/
void QScriptValueIterator::toBack()
{
    d_ptr->toBack();
}

/*!
    Returns the name of the last property that was jumped over using
    next() or previous().

    \sa value(), flags()
*/
QString QScriptValueIterator::name() const
{
    QScriptIsolate api(d_ptr->engine());
    return d_ptr->name();
}

/*!
    Returns the name of the last property that was jumped over using
    next() or previous().
*/
QScriptString QScriptValueIterator::scriptName() const
{
    QScriptIsolate api(d_ptr->engine());
    return QScriptStringPrivate::get(d_ptr->scriptName());
}

/*!
    Returns the value of the last property that was jumped over using
    next() or previous().

    \sa setValue(), name()
*/
QScriptValue QScriptValueIterator::value() const
{
    Q_D(const QScriptValueIterator);
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->value());
}

/*!
    Sets the \a value of the last property that was jumped over using
    next() or previous().

    \sa value(), name()
*/
void QScriptValueIterator::setValue(const QScriptValue& value)
{
    Q_D(QScriptValueIterator);
    QScriptIsolate api(d->engine());
    d->setValue(QScriptValuePrivate::get(value));
}

/*!
    Removes the last property that was jumped over using next()
    or previous().

    \sa setValue()
*/
void QScriptValueIterator::remove()
{
    Q_D(QScriptValueIterator);
    QScriptIsolate api(d->engine());
    d->remove();
}

/*!
    Returns the flags of the last property that was jumped over using
    next() or previous().

    \sa value()
*/
QScriptValue::PropertyFlags QScriptValueIterator::flags() const
{
    Q_D(const QScriptValueIterator);
    QScriptIsolate api(d->engine());
    return d->flags();
}

/*!
    Makes the iterator operate on \a object. The iterator is set to be
    at the front of the sequence of properties (before the first
    property).
*/
QScriptValueIterator& QScriptValueIterator::operator=(QScriptValue& object)
{
    d_ptr.reset(new QScriptValueIteratorPrivate(QScriptValuePrivate::get(object)));
    return *this;
}

QT_END_NAMESPACE
