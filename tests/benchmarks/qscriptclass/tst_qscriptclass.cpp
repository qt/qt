/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtest.h>
#include <QtScript>

Q_DECLARE_METATYPE(QScriptContext*)
Q_DECLARE_METATYPE(QScriptValue)
Q_DECLARE_METATYPE(QScriptValueList)

//TESTED_FILES=

class TestClass : public QScriptClass
{
public:
    struct CustomProperty {
        QueryFlags qflags;
        uint id;
        QScriptValue::PropertyFlags pflags;
        QScriptValue value;

        CustomProperty(QueryFlags qf, uint i, QScriptValue::PropertyFlags pf,
                       const QScriptValue &val)
            : qflags(qf), id(i), pflags(pf), value(val) { }
    };

    enum CallableMode {
        NotCallable,
        CallableReturnsSum,
        CallableReturnsArgument,
        CallableReturnsInvalidVariant
    };

    TestClass(QScriptEngine *engine);
    ~TestClass();

    void addCustomProperty(const QScriptString &name, QueryFlags qflags,
                           uint id, QScriptValue::PropertyFlags pflags,
                           const QScriptValue &value);
    void removeCustomProperty(const QScriptString &name);

    QueryFlags queryProperty(const QScriptValue &object,
                             const QScriptString &name,
                             QueryFlags flags, uint *id);

    QScriptValue property(const QScriptValue &object,
                          const QScriptString &name, uint id);

    void setProperty(QScriptValue &object, const QScriptString &name,
                     uint id, const QScriptValue &value);

    QScriptValue::PropertyFlags propertyFlags(
        const QScriptValue &object, const QScriptString &name, uint id);

    QScriptClassPropertyIterator *newIterator(const QScriptValue &object);

    QScriptValue prototype() const;

    QString name() const;

    bool supportsExtension(Extension extension) const;
    QVariant extension(Extension extension,
                       const QVariant &argument = QVariant());

    void setIterationEnabled(bool enable);
    bool isIterationEnabled() const;

    void setCallableMode(CallableMode mode);
    CallableMode callableMode() const;

    void setHasInstance(bool hasInstance);
    bool hasInstance() const;

private:
    inline CustomProperty *findCustomProperty(const QScriptString &name);

    QHash<QScriptString, CustomProperty*> customProperties;

    QScriptValue m_prototype;
    bool m_iterationEnabled;
    CallableMode m_callableMode;
    bool m_hasInstance;
};

class TestClassPropertyIterator : public QScriptClassPropertyIterator
{
public:
    TestClassPropertyIterator(const QHash<QScriptString, TestClass::CustomProperty*> &props,
                      const QScriptValue &object);
    ~TestClassPropertyIterator();

    bool hasNext() const;
    void next();

    bool hasPrevious() const;
    void previous();

    void toFront();
    void toBack();

    QScriptString name() const;
    uint id() const;
    QScriptValue::PropertyFlags flags() const;

private:
    int m_index;
    int m_last;
    QHash<QScriptString, TestClass::CustomProperty*> m_props;
};

TestClass::TestClass(QScriptEngine *engine)
    : QScriptClass(engine), m_iterationEnabled(true),
      m_callableMode(NotCallable), m_hasInstance(false)
{
    m_prototype = engine->newObject();
}

TestClass::~TestClass()
{
    qDeleteAll(customProperties);
}

TestClass::CustomProperty* TestClass::findCustomProperty(const QScriptString &name)
{
    QHash<QScriptString, CustomProperty*>::const_iterator it;
    it = customProperties.constFind(name);
    if (it == customProperties.constEnd())
        return 0;
    return it.value();

}

void TestClass::addCustomProperty(const QScriptString &name, QueryFlags qflags,
                                  uint id, QScriptValue::PropertyFlags pflags,
                                  const QScriptValue &value)
{
    customProperties.insert(name, new CustomProperty(qflags, id, pflags, value));
}

void TestClass::removeCustomProperty(const QScriptString &name)
{
    CustomProperty *prop = customProperties.take(name);
    if (prop)
        delete prop;
}

QScriptClass::QueryFlags TestClass::queryProperty(const QScriptValue &/*object*/,
                                    const QScriptString &name,
                                    QueryFlags flags, uint *id)
{
    CustomProperty *prop = findCustomProperty(name);
    if (!prop)
        return 0;
    *id = prop->id;
    return prop->qflags & flags;
}

QScriptValue TestClass::property(const QScriptValue &/*object*/,
                                 const QScriptString &name, uint /*id*/)
{
    CustomProperty *prop = findCustomProperty(name);
    if (!prop)
        return QScriptValue();
    return prop->value;
}

void TestClass::setProperty(QScriptValue &/*object*/, const QScriptString &name,
                            uint /*id*/, const QScriptValue &value)
{
    CustomProperty *prop = findCustomProperty(name);
    if (!prop)
        return;
    prop->value = value;
}

QScriptValue::PropertyFlags TestClass::propertyFlags(
    const QScriptValue &/*object*/, const QScriptString &name, uint /*id*/)
{
    CustomProperty *prop = findCustomProperty(name);
    if (!prop)
        return 0;
    return prop->pflags;
}

QScriptClassPropertyIterator *TestClass::newIterator(const QScriptValue &object)
{
    if (!m_iterationEnabled)
        return 0;
    return new TestClassPropertyIterator(customProperties, object);
}

QScriptValue TestClass::prototype() const
{
    return m_prototype;
}

QString TestClass::name() const
{
    return QLatin1String("TestClass");
}

bool TestClass::supportsExtension(Extension extension) const
{
    if (extension == Callable)
        return (m_callableMode != NotCallable);
    if (extension == HasInstance)
        return m_hasInstance;
    return false;
}

QVariant TestClass::extension(Extension extension,
                              const QVariant &argument)
{
    if (extension == Callable) {
        Q_ASSERT(m_callableMode != NotCallable);
        QScriptContext *ctx = qvariant_cast<QScriptContext*>(argument);
        if (m_callableMode == CallableReturnsSum) {
            qsreal sum = 0;
            for (int i = 0; i < ctx->argumentCount(); ++i)
                sum += ctx->argument(i).toNumber();
            QScriptValueIterator it(ctx->thisObject());
            while (it.hasNext()) {
                it.next();
                sum += it.value().toNumber();
            }
            return sum;
        } else if (m_callableMode == CallableReturnsArgument) {
            return qVariantFromValue(ctx->argument(0));
        } else if (m_callableMode == CallableReturnsInvalidVariant) {
            return QVariant();
        }
    } else if (extension == HasInstance) {
        Q_ASSERT(m_hasInstance);
        QScriptValueList args = qvariant_cast<QScriptValueList>(argument);
        QScriptValue obj = args.at(0);
        QScriptValue value = args.at(1);
        return value.property("foo").equals(obj.property("foo"));
    }
    return QVariant();
}

void TestClass::setIterationEnabled(bool enable)
{
    m_iterationEnabled = enable;
}

bool TestClass::isIterationEnabled() const
{
    return m_iterationEnabled;
}

void TestClass::setCallableMode(CallableMode mode)
{
    m_callableMode = mode;
}

TestClass::CallableMode TestClass::callableMode() const
{
    return m_callableMode;
}

void TestClass::setHasInstance(bool hasInstance)
{
    m_hasInstance = hasInstance;
}

bool TestClass::hasInstance() const
{
    return m_hasInstance;
}

TestClassPropertyIterator::TestClassPropertyIterator(const QHash<QScriptString, TestClass::CustomProperty*> &props,
                                     const QScriptValue &object)
    : QScriptClassPropertyIterator(object)
{
    m_props = props;
    toFront();
}

TestClassPropertyIterator::~TestClassPropertyIterator()
{
}

bool TestClassPropertyIterator::hasNext() const
{
    return m_index < m_props.size();
}

void TestClassPropertyIterator::next()
{
    m_last = m_index;
    ++m_index;
}

bool TestClassPropertyIterator::hasPrevious() const
{
    return m_index > 0;
}

void TestClassPropertyIterator::previous()
{
    --m_index;
    m_last = m_index;
}

void TestClassPropertyIterator::toFront()
{
    m_index = 0;
    m_last = -1;
}

void TestClassPropertyIterator::toBack()
{
    m_index = m_props.size();
    m_last = -1;
}

QScriptString TestClassPropertyIterator::name() const
{
    return m_props.keys().value(m_last);
}

uint TestClassPropertyIterator::id() const
{
    QScriptString key = m_props.keys().value(m_last);
    if (!key.isValid())
        return 0;
    TestClass::CustomProperty *prop = m_props.value(key);
    return prop->id;
}

QScriptValue::PropertyFlags TestClassPropertyIterator::flags() const
{
    QScriptString key = m_props.keys().value(m_last);
    if (!key.isValid())
        return 0;
    TestClass::CustomProperty *prop = m_props.value(key);
    return prop->pflags;
}

class tst_QScriptClass : public QObject
{
    Q_OBJECT

public:
    tst_QScriptClass();
    virtual ~tst_QScriptClass();

public slots:
    void init();
    void cleanup();

private slots:
    void noSuchProperty();
    void property();
    void setProperty();
    void propertyFlags();
    void call();
    void hasInstance();
    void iterate();
};

tst_QScriptClass::tst_QScriptClass()
{
}

tst_QScriptClass::~tst_QScriptClass()
{
}

void tst_QScriptClass::init()
{
}

void tst_QScriptClass::cleanup()
{
}

void tst_QScriptClass::noSuchProperty()
{
    QScriptEngine eng;
    TestClass cls(&eng);
    QScriptValue obj = eng.newObject(&cls);
    QString propertyName = QString::fromLatin1("foo");
    QBENCHMARK {
        (void)obj.property(propertyName);
    }
}

void tst_QScriptClass::property()
{
    QScriptEngine eng;
    TestClass cls(&eng);
    QScriptString foo = eng.toStringHandle("foo");
    cls.addCustomProperty(foo, QScriptClass::HandlesReadAccess, /*id=*/1, /*attributes=*/0, /*value=*/123);
    QScriptValue obj = eng.newObject(&cls);
    QBENCHMARK {
        (void)obj.property(foo);
    }
}

void tst_QScriptClass::setProperty()
{
    QScriptEngine eng;
    TestClass cls(&eng);
    QScriptString foo = eng.toStringHandle("foo");
    cls.addCustomProperty(foo, QScriptClass::HandlesWriteAccess, /*id=*/1, /*attributes=*/0, /*value=*/123);
    QScriptValue obj = eng.newObject(&cls);
    QScriptValue value(456);
    QBENCHMARK {
        obj.setProperty(foo, value);
    }
}

void tst_QScriptClass::propertyFlags()
{
    QScriptEngine eng;
    TestClass cls(&eng);
    QScriptString foo = eng.toStringHandle("foo");
    cls.addCustomProperty(foo, QScriptClass::HandlesReadAccess, /*id=*/1, QScriptValue::ReadOnly, /*value=*/123);
    QScriptValue obj = eng.newObject(&cls);
    QBENCHMARK {
        (void)obj.propertyFlags(foo);
    }
}

void tst_QScriptClass::call()
{
    QScriptEngine eng;
    TestClass cls(&eng);
    cls.setCallableMode(TestClass::CallableReturnsArgument);
    QScriptValue obj = eng.newObject(&cls);
    QScriptValue thisObject;
    QScriptValueList args;
    args.append(123);
    QBENCHMARK {
        (void)obj.call(thisObject, args);
    }
}

void tst_QScriptClass::hasInstance()
{
    QScriptEngine eng;
    TestClass cls(&eng);
    cls.setHasInstance(true);
    QScriptValue obj = eng.newObject(&cls);
    obj.setProperty("foo", 123);
    QScriptValue plain = eng.newObject();
    plain.setProperty("foo", obj.property("foo"));
    QBENCHMARK {
        (void)plain.instanceOf(obj);
    }
}

void tst_QScriptClass::iterate()
{
    QScriptEngine eng;
    TestClass cls(&eng);
    cls.setIterationEnabled(true);
    cls.addCustomProperty(eng.toStringHandle("foo"), QScriptClass::HandlesReadAccess, /*id=*/1, /*attributes=*/0, /*value=*/123);
    cls.addCustomProperty(eng.toStringHandle("bar"), QScriptClass::HandlesReadAccess, /*id=*/2, /*attributes=*/0, /*value=*/456);
    QScriptValue obj = eng.newObject(&cls);
    QBENCHMARK {
        QScriptValueIterator it(obj);
        while (it.hasNext()) {
            it.next();
            (void)it.scriptName();
        }
    }
}

QTEST_MAIN(tst_QScriptClass)
#include "tst_qscriptclass.moc"
