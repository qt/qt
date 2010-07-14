/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "private/qdeclarativelistmodel_p_p.h"
#include "private/qdeclarativelistmodelworkeragent_p.h"
#include "private/qdeclarativeopenmetaobject_p.h"

#include <qdeclarativecustomparser_p.h>
#include <qdeclarativeparser_p.h>
#include <qdeclarativeengine_p.h>
#include <qdeclarativecontext.h>
#include <qdeclarativeinfo.h>

#include <QtCore/qdebug.h>
#include <QtCore/qstack.h>
#include <QXmlStreamReader>
#include <QtScript/qscriptvalueiterator.h>

Q_DECLARE_METATYPE(QListModelInterface *)

QT_BEGIN_NAMESPACE

#define DATA_ROLE_ID 1
#define DATA_ROLE_NAME "data"

QDeclarativeListModelParser::ListInstruction *QDeclarativeListModelParser::ListModelData::instructions() const
{
    return (QDeclarativeListModelParser::ListInstruction *)((char *)this + sizeof(ListModelData));
}

/*!
    \qmlclass ListModel QDeclarativeListModel
    \since 4.7
    \brief The ListModel element defines a free-form list data source.

    The ListModel is a simple hierarchy of elements containing data roles. The contents can
    be defined dynamically, or explicitly in QML:

    For example:

    \snippet doc/src/snippets/declarative/listmodel.qml 0

    Roles (properties) must begin with a lower-case letter. The above example defines a
    ListModel containing three elements, with the roles "name" and "cost".

    Values must be simple constants - either strings (quoted and optionally within a call to QT_TR_NOOP),
    bools (true, false), numbers, or enum values (like Text.AlignHCenter).

    The defined model can be used in views such as ListView:

    \snippet doc/src/snippets/declarative/listmodel-simple.qml 0
    \dots 8
    \snippet doc/src/snippets/declarative/listmodel-simple.qml 1
    \image listmodel.png

    It is possible for roles to contain list data.  In the example below we create a list of fruit attributes:

    \snippet doc/src/snippets/declarative/listmodel-nested.qml model

    The delegate below displays all the fruit attributes:

    \snippet doc/src/snippets/declarative/listmodel-nested.qml delegate
    \image listmodel-nested.png


    \section2 Modifying list models

    The content of a ListModel may be created and modified using the clear(),
    append(), set() and setProperty() methods.  For example:
    
    \snippet doc/src/snippets/declarative/listmodel-modify.qml delegate

    When creating content dynamically, note that the set of available properties cannot be changed
    except by first clearing the model. Whatever properties are first added to the model are then the
    only permitted properties in the model until it is cleared.


    \section2 Using threaded list models with WorkerScript

    ListModel can be used together with WorkerScript access a list model
    from multiple threads. This is useful if list modifications are
    synchronous and take some time: the list operations can be moved to a
    different thread to avoid blocking of the main GUI thread.

    Here is an example that uses WorkerScript to periodically append the
    current time to a list model:

    \snippet examples/declarative/threading/threadedlistmodel/timedisplay.qml 0

    The included file, \tt dataloader.js, looks like this:

    \snippet examples/declarative/threading/threadedlistmodel/dataloader.js 0

    The application's \tt Timer object periodically sends a message to the
    worker script by calling \l WorkerScript::sendMessage(). When this message
    is received, \l {WorkerScript::onMessage}{WorkerScript.onMessage()} is invoked in
    \tt dataloader.js, which appends the current time to the list model.

    Note the call to sync() from the \l {WorkerScript::onMessage}{WorkerScript.onMessage()} handler.
    You must call sync() or else the changes made to the list from the external
    thread will not be reflected in the list model in the main thread.

    \section3 Limitations

    If a list model is to be accessed from a WorkerScript, it \bold cannot
    contain list data. So, the following model cannot be used from a WorkerScript
    because of the list contained in the "attributes" property:

    \code
    ListModel {
        id: fruitModel
        ListElement {
            name: "Apple"
            cost: 2.45
            attributes: [
                ListElement { description: "Core" },
                ListElement { description: "Deciduous" }
            ]
        }
    }
    \endcode

    In addition, the WorkerScript cannot add any list data to the model.

    \sa {qmlmodels}{Data Models}, {declarative/threading/threadedlistmodel}{Threaded ListModel example}, QtDeclarative
*/


/*
    A ListModel internally uses either a NestedListModel or FlatListModel.

    A NestedListModel can contain lists of ListElements (which
    when retrieved from get() is accessible as a list model within the list
    model) whereas a FlatListModel cannot.

    ListModel uses a NestedListModel to begin with, and if the model is later 
    used from a WorkerScript, it changes to use a FlatListModel instead. This
    is because ModelNode (which abstracts the nested list model data) needs
    access to the declarative engine and script engine, which cannot be
    safely used from outside of the main thread.
*/

QDeclarativeListModel::QDeclarativeListModel(QObject *parent)
: QListModelInterface(parent), m_agent(0), m_nested(new NestedListModel(this)), m_flat(0), m_isWorkerCopy(false)
{
}

QDeclarativeListModel::QDeclarativeListModel(bool workerCopy, QObject *parent)
: QListModelInterface(parent), m_agent(0), m_nested(0), m_flat(0), m_isWorkerCopy(workerCopy)
{
    if (workerCopy)
        m_flat = new FlatListModel(this);
    else
        m_nested = new NestedListModel(this);
}

QDeclarativeListModel::~QDeclarativeListModel()
{
    if (m_agent)
        m_agent->release();

    delete m_nested;
    delete m_flat;
}

bool QDeclarativeListModel::flatten()
{
    if (m_flat)
        return true;

    QList<int> roles = m_nested->roles();

    QList<QHash<int, QVariant> > values;
    bool hasNested = false;
    for (int i=0; i<m_nested->count(); i++) {
        values.append(m_nested->data(i, roles, &hasNested));
        if (hasNested)
            return false;
    }

    FlatListModel *flat = new FlatListModel(this);
    flat->m_values = values;

    for (int i=0; i<roles.count(); i++) {
        QString s = m_nested->toString(roles[i]);
        flat->m_roles.insert(roles[i], s);
        flat->m_strings.insert(s, roles[i]);
    }

    m_flat = flat;
    delete m_nested;
    m_nested = 0;
    return true;
}

QDeclarativeListModelWorkerAgent *QDeclarativeListModel::agent()
{
    if (m_agent)
        return m_agent;

    if (!flatten()) {
        qmlInfo(this) << "List contains nested list values and cannot be used from a worker script";
        return 0;
    }

    m_agent = new QDeclarativeListModelWorkerAgent(this);
    return m_agent;
}

QList<int> QDeclarativeListModel::roles() const
{
    return m_flat ? m_flat->roles() : m_nested->roles();
}

QString QDeclarativeListModel::toString(int role) const
{
    return m_flat ? m_flat->toString(role) : m_nested->toString(role);
}

QHash<int,QVariant> QDeclarativeListModel::data(int index, const QList<int> &roles) const
{
    if (index >= count() || index < 0)
        return QHash<int, QVariant>();

    return m_flat ? m_flat->data(index, roles) : m_nested->data(index, roles);
}

QVariant QDeclarativeListModel::data(int index, int role) const
{
    if (index >= count() || index < 0)
        return QVariant();

    return m_flat ? m_flat->data(index, role) : m_nested->data(index, role);
}

/*!
    \qmlproperty int ListModel::count
    The number of data entries in the model.
*/
int QDeclarativeListModel::count() const
{
    return m_flat ? m_flat->count() : m_nested->count();
}

/*!
    \qmlmethod ListModel::clear()

    Deletes all content from the model. The properties are cleared such that
    different properties may be set on subsequent additions.

    \sa append() remove()
*/
void QDeclarativeListModel::clear()
{
    int cleared = count();
    if (m_flat)
        m_flat->clear();
    else
        m_nested->clear();

    if (!m_isWorkerCopy) {
        emit itemsRemoved(0, cleared);
        emit countChanged();
    }
}

/*!
    \qmlmethod ListModel::remove(int index)

    Deletes the content at \a index from the model.

    \sa clear()
*/
void QDeclarativeListModel::remove(int index)
{
    if (index < 0 || index >= count()) {
        qmlInfo(this) << tr("remove: index %1 out of range").arg(index);
        return;
    }

    if (m_flat)
        m_flat->remove(index);
    else
        m_nested->remove(index);

    if (!m_isWorkerCopy) {
        emit itemsRemoved(index, 1);
        emit countChanged();
    }
}

/*!
    \qmlmethod ListModel::insert(int index, jsobject dict)

    Adds a new item to the list model at position \a index, with the
    values in \a dict.

    \code
        fruitModel.insert(2, {"cost": 5.95, "name":"Pizza"})
    \endcode

    The \a index must be to an existing item in the list, or one past
    the end of the list (equivalent to append).

    \sa set() append()
*/
void QDeclarativeListModel::insert(int index, const QScriptValue& valuemap)
{
    if (!valuemap.isObject() || valuemap.isArray()) {
        qmlInfo(this) << tr("insert: value is not an object");
        return;
    }

    if (index < 0 || index > count()) {
        qmlInfo(this) << tr("insert: index %1 out of range").arg(index);
        return;
    }

    bool ok = m_flat ?  m_flat->insert(index, valuemap) : m_nested->insert(index, valuemap);
    if (ok && !m_isWorkerCopy) {
        emit itemsInserted(index, 1);
        emit countChanged();
    }
}

/*!
    \qmlmethod ListModel::move(int from, int to, int n)

    Moves \a n items \a from one position \a to another.

    The from and to ranges must exist; for example, to move the first 3 items
    to the end of the list:

    \code
        fruitModel.move(0, fruitModel.count - 3, 3)
    \endcode

    \sa append()
*/
void QDeclarativeListModel::move(int from, int to, int n)
{
    if (n==0 || from==to)
        return;
    if (from+n > count() || to+n > count() || from < 0 || to < 0 || n < 0) {
        qmlInfo(this) << tr("move: out of range");
        return;
    }

    int origfrom = from;
    int origto = to;
    int orign = n;
    if (from > to) {
        // Only move forwards - flip if backwards moving
        int tfrom = from;
        int tto = to;
        from = tto;
        to = tto+n;
        n = tfrom-tto;
    }

    if (m_flat)
        m_flat->move(from, to, n);
    else
        m_nested->move(from, to, n);

    if (!m_isWorkerCopy)
        emit itemsMoved(origfrom, origto, orign);
}

/*!
    \qmlmethod ListModel::append(jsobject dict)

    Adds a new item to the end of the list model, with the
    values in \a dict.

    \code
        fruitModel.append({"cost": 5.95, "name":"Pizza"})
    \endcode

    \sa set() remove()
*/
void QDeclarativeListModel::append(const QScriptValue& valuemap)
{
    if (!valuemap.isObject() || valuemap.isArray()) {
        qmlInfo(this) << tr("append: value is not an object");
        return;
    }

    insert(count(), valuemap);
}

/*!
    \qmlmethod object ListModel::get(int index)

    Returns the item at \a index in the list model.

    \code
        fruitModel.append({"cost": 5.95, "name":"Jackfruit"})
        fruitModel.get(0).cost
    \endcode

    The \a index must be an element in the list.

    Note that properties of the returned object that are themselves objects
    will also be models, and this get() method is used to access elements:

    \code
        fruitModel.append(..., "attributes":
            [{"name":"spikes","value":"7mm"},
             {"name":"color","value":"green"}]);
        fruitModel.get(0).attributes.get(1).value; // == "green"
    \endcode

    \sa append()
*/
QScriptValue QDeclarativeListModel::get(int index) const
{
    // the internal flat/nested class checks for bad index
    return m_flat ? m_flat->get(index) : m_nested->get(index);
}

/*!
    \qmlmethod ListModel::set(int index, jsobject dict)

    Changes the item at \a index in the list model with the
    values in \a dict. Properties not appearing in \a dict
    are left unchanged.

    \code
        fruitModel.set(3, {"cost": 5.95, "name":"Pizza"})
    \endcode

    The \a index must be an element in the list.

    \sa append()
*/
void QDeclarativeListModel::set(int index, const QScriptValue& valuemap)
{
    if (!valuemap.isObject() || valuemap.isArray()) {
        qmlInfo(this) << tr("set: value is not an object");
        return;
    }
    if (count() == 0 || index > count() || index < 0) {
        qmlInfo(this) << tr("set: index %1 out of range").arg(index);
        return;
    }

    if (index == count()) {
        append(valuemap);
    } else {
        QList<int> roles;
        if (m_flat)
            m_flat->set(index, valuemap, &roles);
        else
            m_nested->set(index, valuemap, &roles);

        if (!m_isWorkerCopy)
            emit itemsChanged(index, 1, roles);
    }
}

/*!
    \qmlmethod ListModel::setProperty(int index, string property, variant value)

    Changes the \a property of the item at \a index in the list model to \a value.

    \code
        fruitModel.setProperty(3, "cost", 5.95)
    \endcode

    The \a index must be an element in the list.

    \sa append()
*/
void QDeclarativeListModel::setProperty(int index, const QString& property, const QVariant& value)
{
    if (count() == 0 || index >= count() || index < 0) {
        qmlInfo(this) << tr("set: index %1 out of range").arg(index);
        return;
    }

    QList<int> roles;
    if (m_flat)
        m_flat->setProperty(index, property, value, &roles);
    else
        m_nested->setProperty(index, property, value, &roles);

    if (!m_isWorkerCopy)
        emit itemsChanged(index, 1, roles);
}

/*!
    \qmlmethod ListModel::sync()

    Writes any unsaved changes to the list model after it has been modified
    from a worker script.
*/
void QDeclarativeListModel::sync()
{
    // This is just a dummy method to make it look like sync() exists in
    // ListModel (and not just QDeclarativeListModelWorkerAgent) and to let
    // us document sync().
    qmlInfo(this) << "List sync() can only be called from a WorkerScript";
}

bool QDeclarativeListModelParser::compileProperty(const QDeclarativeCustomParserProperty &prop, QList<ListInstruction> &instr, QByteArray &data)
{
    QList<QVariant> values = prop.assignedValues();
    for(int ii = 0; ii < values.count(); ++ii) {
        const QVariant &value = values.at(ii);

        if(value.userType() == qMetaTypeId<QDeclarativeCustomParserNode>()) {
            QDeclarativeCustomParserNode node =
                qvariant_cast<QDeclarativeCustomParserNode>(value);

            if (node.name() != listElementTypeName) {
                const QMetaObject *mo = resolveType(node.name());
                if (mo != &QDeclarativeListElement::staticMetaObject) {
                    error(node, QDeclarativeListModel::tr("ListElement: cannot contain nested elements"));
                    return false;
                }
                listElementTypeName = node.name(); // cache right name for next time
            }

            {
            ListInstruction li;
            li.type = ListInstruction::Push;
            li.dataIdx = -1;
            instr << li;
            }

            QList<QDeclarativeCustomParserProperty> props = node.properties();
            for(int jj = 0; jj < props.count(); ++jj) {
                const QDeclarativeCustomParserProperty &nodeProp = props.at(jj);
                if (nodeProp.name() == "") {
                    error(nodeProp, QDeclarativeListModel::tr("ListElement: cannot contain nested elements"));
                    return false;
                }
                if (nodeProp.name() == "id") {
                    error(nodeProp, QDeclarativeListModel::tr("ListElement: cannot use reserved \"id\" property"));
                    return false;
                }

                ListInstruction li;
                int ref = data.count();
                data.append(nodeProp.name());
                data.append('\0');
                li.type = ListInstruction::Set;
                li.dataIdx = ref;
                instr << li;

                if(!compileProperty(nodeProp, instr, data))
                    return false;

                li.type = ListInstruction::Pop;
                li.dataIdx = -1;
                instr << li;
            }

            {
            ListInstruction li;
            li.type = ListInstruction::Pop;
            li.dataIdx = -1;
            instr << li;
            }

        } else {

            QDeclarativeParser::Variant variant =
                qvariant_cast<QDeclarativeParser::Variant>(value);

            int ref = data.count();

            QByteArray d;
            d += char(variant.type()); // type tag
            if (variant.isString()) {
                d += variant.asString().toUtf8();
            } else if (variant.isNumber()) {
                d += QByteArray::number(variant.asNumber(),'g',20);
            } else if (variant.isBoolean()) {
                d += char(variant.asBoolean());
            } else if (variant.isScript()) {
                if (definesEmptyList(variant.asScript())) {
                    d[0] = char(QDeclarativeParser::Variant::Invalid); // marks empty list
                } else {
                    QByteArray script = variant.asScript().toUtf8();
                    int v = evaluateEnum(script);
                    if (v<0) {
                        if (script.startsWith("QT_TR_NOOP(\"") && script.endsWith("\")")) {
                            d[0] = char(QDeclarativeParser::Variant::String);
                            d += script.mid(12,script.length()-14);
                        } else {
                            error(prop, QDeclarativeListModel::tr("ListElement: cannot use script for property value"));
                            return false;
                        }
                    } else {
                        d[0] = char(QDeclarativeParser::Variant::Number);
                        d += QByteArray::number(v);
                    }
                }
            }
            d.append('\0');
            data.append(d);

            ListInstruction li;
            li.type = ListInstruction::Value;
            li.dataIdx = ref;
            instr << li;
        }
    }

    return true;
}

QByteArray QDeclarativeListModelParser::compile(const QList<QDeclarativeCustomParserProperty> &customProps)
{
    QList<ListInstruction> instr;
    QByteArray data;
    listElementTypeName = QByteArray(); // unknown

    for(int ii = 0; ii < customProps.count(); ++ii) {
        const QDeclarativeCustomParserProperty &prop = customProps.at(ii);
        if(prop.name() != "") { // isn't default property
            error(prop, QDeclarativeListModel::tr("ListModel: undefined property '%1'").arg(QString::fromUtf8(prop.name())));
            return QByteArray();
        }

        if(!compileProperty(prop, instr, data)) {
            return QByteArray();
        }
    }

    int size = sizeof(ListModelData) +
               instr.count() * sizeof(ListInstruction) +
               data.count();

    QByteArray rv;
    rv.resize(size);

    ListModelData *lmd = (ListModelData *)rv.data();
    lmd->dataOffset = sizeof(ListModelData) +
                     instr.count() * sizeof(ListInstruction);
    lmd->instrCount = instr.count();
    for (int ii = 0; ii < instr.count(); ++ii)
        lmd->instructions()[ii] = instr.at(ii);
    ::memcpy(rv.data() + lmd->dataOffset, data.constData(), data.count());

    return rv;
}

void QDeclarativeListModelParser::setCustomData(QObject *obj, const QByteArray &d)
{
    QDeclarativeListModel *rv = static_cast<QDeclarativeListModel *>(obj);

    ModelNode *root = new ModelNode;
    rv->m_nested->_root = root;
    QStack<ModelNode *> nodes;
    nodes << root;

    bool processingSet = false;

    const ListModelData *lmd = (const ListModelData *)d.constData();
    const char *data = ((const char *)lmd) + lmd->dataOffset;

    for (int ii = 0; ii < lmd->instrCount; ++ii) {
        const ListInstruction &instr = lmd->instructions()[ii];

        switch(instr.type) {
        case ListInstruction::Push:
            {
                ModelNode *n = nodes.top();
                ModelNode *n2 = new ModelNode;
                n->values << qVariantFromValue(n2);
                nodes.push(n2);
                if (processingSet)
                    n->isArray = true;
            }
            break;

        case ListInstruction::Pop:
            nodes.pop();
            break;

        case ListInstruction::Value:
            {
                ModelNode *n = nodes.top();
                switch (QDeclarativeParser::Variant::Type(data[instr.dataIdx])) {
                 case QDeclarativeParser::Variant::Invalid:
                    n->isArray = true;
                    break;
                 case QDeclarativeParser::Variant::Boolean:
                    n->values.append(bool(data[1 + instr.dataIdx]));
                    break;
                 case QDeclarativeParser::Variant::Number:
                    n->values.append(QByteArray(data + 1 + instr.dataIdx).toDouble());
                    break;
                 case QDeclarativeParser::Variant::String:
                    n->values.append(QString::fromUtf8(data + 1 + instr.dataIdx));
                    break;
                 default:
                    Q_ASSERT("Format error in ListInstruction");
                }

                processingSet = false;
            }
            break;

        case ListInstruction::Set:
            {
                ModelNode *n = nodes.top();
                ModelNode *n2 = new ModelNode;
                n->properties.insert(QString::fromUtf8(data + instr.dataIdx), n2);
                nodes.push(n2);
                processingSet = true;
            }
            break;
        }
    }
}

bool QDeclarativeListModelParser::definesEmptyList(const QString &s)
{
    if (s.startsWith(QLatin1Char('[')) && s.endsWith(QLatin1Char(']'))) {
        for (int i=1; i<s.length()-1; i++) {
            if (!s[i].isSpace())
                return false;
        }
        return true;
    }
    return false;
}

/*!
    \qmlclass ListElement
    \since 4.7
    \brief The ListElement element defines a data item in a ListModel.

    \sa ListModel
*/

FlatListModel::FlatListModel(QDeclarativeListModel *base)
    : m_scriptEngine(0), m_listModel(base)
{
}

FlatListModel::~FlatListModel()
{
}

QHash<int,QVariant> FlatListModel::data(int index, const QList<int> &roles) const
{
    Q_ASSERT(index >= 0 && index < m_values.count());

    QHash<int, QVariant> row;
    for (int i=0; i<roles.count(); i++) {
        int role = roles[i];
        if (m_values[index].contains(role))
            row.insert(role, m_values[index][role]);
    }
    return row;
}

QVariant FlatListModel::data(int index, int role) const
{
    Q_ASSERT(index >= 0 && index < m_values.count());
    if (m_values[index].contains(role))
        return m_values[index][role];
    return QVariant();
}

QList<int> FlatListModel::roles() const
{
    return m_roles.keys();
}

QString FlatListModel::toString(int role) const
{
    if (m_roles.contains(role))
        return m_roles[role];
    return QString();
}

int FlatListModel::count() const
{
    return m_values.count();
}

void FlatListModel::clear()
{
    m_values.clear();
}

void FlatListModel::remove(int index)
{
    m_values.removeAt(index);
}

bool FlatListModel::append(const QScriptValue &value)
{
    return insert(m_values.count(), value);
}

bool FlatListModel::insert(int index, const QScriptValue &value)
{
    Q_ASSERT(index >= 0 && index <= m_values.count());

    QHash<int, QVariant> row;
    if (!addValue(value, &row, 0))
        return false;

    m_values.insert(index, row);
    return true;
}

QScriptValue FlatListModel::get(int index) const
{
    QScriptEngine *scriptEngine = m_scriptEngine ? m_scriptEngine : QDeclarativeEnginePrivate::getScriptEngine(qmlEngine(m_listModel));

    if (!scriptEngine) 
        return 0;

    if (index < 0 || index >= m_values.count())
        return scriptEngine->undefinedValue();

    QScriptValue rv = scriptEngine->newObject();

    QHash<int, QVariant> row = m_values.at(index);
    for (QHash<int, QVariant>::ConstIterator iter = row.begin(); iter != row.end(); ++iter)
        rv.setProperty(m_roles.value(iter.key()), qScriptValueFromValue(scriptEngine, iter.value()));

    return rv;
}

void FlatListModel::set(int index, const QScriptValue &value, QList<int> *roles)
{
    Q_ASSERT(index >= 0 && index < m_values.count());

    QHash<int, QVariant> row = m_values[index];
    if (addValue(value, &row, roles))
        m_values[index] = row;
}

void FlatListModel::setProperty(int index, const QString& property, const QVariant& value, QList<int> *roles)
{
    Q_ASSERT(index >= 0 && index < m_values.count());

    QHash<QString, int>::Iterator iter = m_strings.find(property);
    int role;
    if (iter == m_strings.end()) {
        role = m_roles.count();
        m_roles.insert(role, property);
        m_strings.insert(property, role);
    } else {
        role = iter.value();
    }
    roles->append(role);

    m_values[index][role] = value;
}

void FlatListModel::move(int from, int to, int n)
{
    if (n == 1) {
        m_values.move(from, to);
    } else {
        QList<QHash<int, QVariant> > replaced;
        int i=0;
        QList<QHash<int, QVariant> >::ConstIterator it=m_values.begin(); it += from+n;
        for (; i<to-from; ++i,++it)
            replaced.append(*it);
        i=0;
        it=m_values.begin(); it += from;
        for (; i<n; ++i,++it)
            replaced.append(*it);
        QList<QHash<int, QVariant> >::ConstIterator f=replaced.begin();
        QList<QHash<int, QVariant> >::Iterator t=m_values.begin(); t += from;
        for (; f != replaced.end(); ++f, ++t)
            *t = *f;
    }
}

bool FlatListModel::addValue(const QScriptValue &value, QHash<int, QVariant> *row, QList<int> *roles)
{
    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QScriptValue value = it.value();
        if (!value.isVariant() && !value.isRegExp() && !value.isDate() && value.isObject()) {
            qmlInfo(m_listModel) << "Cannot add nested list values when modifying or after modification from a worker script";
            return false;
        }

        QString name = it.name();
        QVariant v = it.value().toVariant();

        QHash<QString, int>::Iterator iter = m_strings.find(name);
        if (iter == m_strings.end()) {
            int role = m_roles.count();
            m_roles.insert(role, name);
            iter = m_strings.insert(name, role);
            if (roles)
                roles->append(role);
        }
        row->insert(*iter, v);
    }
    return true;
}

NestedListModel::NestedListModel(QDeclarativeListModel *base)
    : _root(0), m_listModel(base), _rolesOk(false)
{
}

NestedListModel::~NestedListModel()
{
    delete _root;
}

QVariant NestedListModel::valueForNode(ModelNode *node, bool *hasNested) const
{
    QObject *rv = 0;
    if (hasNested)
        *hasNested = false;

    if (node->isArray) {
        // List
        rv = node->model(this);
        if (hasNested)
            *hasNested = true;
    } else {
        if (!node->properties.isEmpty()) {
            // Object
            rv = node->object(this);
        } else if (node->values.count() == 0) {
            // Invalid
            return QVariant();
        } else if (node->values.count() == 1) {
            // Value
            QVariant &var = node->values[0];
            ModelNode *valueNode = qvariant_cast<ModelNode *>(var);
            if (valueNode) {
                if (!valueNode->properties.isEmpty())
                    rv = valueNode->object(this);
                else
                    rv = valueNode->model(this);
            } else {
                return var;
            }
        }
    }

    if (rv) {
        return QVariant::fromValue(rv);
    } else {
        return QVariant();
    }
}

QHash<int,QVariant> NestedListModel::data(int index, const QList<int> &roles, bool *hasNested) const
{
    Q_ASSERT(_root && index >= 0 && index < _root->values.count());
    checkRoles();
    QHash<int, QVariant> rv;

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    if (!node)
        return rv;

    for (int ii = 0; ii < roles.count(); ++ii) {
        const QString &roleString = roleStrings.at(roles.at(ii));

        QHash<QString, ModelNode *>::ConstIterator iter = node->properties.find(roleString);
        if (iter != node->properties.end()) {
            ModelNode *row = *iter;
            rv.insert(roles.at(ii), valueForNode(row, hasNested));
        }
    }

    return rv;
}

QVariant NestedListModel::data(int index, int role) const
{
    Q_ASSERT(_root && index >= 0 && index < _root->values.count());
    checkRoles();
    QVariant rv;
    if (roleStrings.count() < role)
        return rv;

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    if (!node)
        return rv;

    const QString &roleString = roleStrings.at(role);

    QHash<QString, ModelNode *>::ConstIterator iter = node->properties.find(roleString);
    if (iter != node->properties.end()) {
        ModelNode *row = *iter;
        rv = valueForNode(row);
    }

    return rv;
}

int NestedListModel::count() const
{
    if (!_root) return 0;
    return _root->values.count();
}

void NestedListModel::clear()
{
    _rolesOk = false;
    roleStrings.clear();

    delete _root;
    _root = 0;
}

void NestedListModel::remove(int index)
{
    if (!_root)
        return;
    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    _root->values.removeAt(index);
    if (node)
        delete node;
}

bool NestedListModel::insert(int index, const QScriptValue& valuemap)
{
    if (!_root)
        _root = new ModelNode;

    ModelNode *mn = new ModelNode;
    mn->setObjectValue(valuemap);
    _root->values.insert(index,qVariantFromValue(mn));
    return true;
}

void NestedListModel::move(int from, int to, int n)
{
    if (n==1) {
        _root->values.move(from,to);
    } else {
        QList<QVariant> replaced;
        int i=0;
        QVariantList::const_iterator it=_root->values.begin(); it += from+n;
        for (; i<to-from; ++i,++it)
            replaced.append(*it);
        i=0;
        it=_root->values.begin(); it += from;
        for (; i<n; ++i,++it)
            replaced.append(*it);
        QVariantList::const_iterator f=replaced.begin();
        QVariantList::iterator t=_root->values.begin(); t += from;
        for (; f != replaced.end(); ++f, ++t)
            *t = *f;
    }
}

bool NestedListModel::append(const QScriptValue& valuemap)
{
    if (!_root)
        _root = new ModelNode;
    ModelNode *mn = new ModelNode;
    mn->setObjectValue(valuemap);
    _root->values << qVariantFromValue(mn);
    return true;
}

QScriptValue NestedListModel::get(int index) const
{   
    QDeclarativeEngine *eng = qmlEngine(m_listModel);
    if (!eng) 
        return 0;

    if (index < 0 || index >= count()) {
        QScriptEngine *seng = QDeclarativeEnginePrivate::getScriptEngine(eng);
        if (seng)
            return seng->undefinedValue();
        return 0;
    }

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    if (!node)
        return 0;
    
    return QDeclarativeEnginePrivate::qmlScriptObject(node->object(this), eng);
}

void NestedListModel::set(int index, const QScriptValue& valuemap, QList<int> *roles)
{
    Q_ASSERT(index >=0 && index < count());

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    node->setObjectValue(valuemap);

    QScriptValueIterator it(valuemap);
    while (it.hasNext()) {
        it.next();
        int r = roleStrings.indexOf(it.name());
        if (r < 0) {
            r = roleStrings.count();
            roleStrings << it.name();
        }
        roles->append(r);
    }
}

void NestedListModel::setProperty(int index, const QString& property, const QVariant& value, QList<int> *roles)
{
    Q_ASSERT(index >=0 && index < count());

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    node->setProperty(property, value);

    int r = roleStrings.indexOf(property);
    if (r < 0) {
        r = roleStrings.count();
        roleStrings << property;
    }
    roles->append(r);
}

void NestedListModel::checkRoles() const
{
    if (_rolesOk || !_root)
        return;

    for (int i = 0; i<_root->values.count(); ++i) {
        ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(i));
        if (node) {
            foreach (const QString &role, node->properties.keys()) {
                if (!roleStrings.contains(role))
                    roleStrings.append(role);
            }
        }
    }

    _rolesOk = true;
}

QList<int> NestedListModel::roles() const
{
    checkRoles();
    QList<int> rv;
    for (int ii = 0; ii < roleStrings.count(); ++ii)
        rv << ii;
    return rv;
}

QString NestedListModel::toString(int role) const
{
    checkRoles();
    if (role < roleStrings.count())
        return roleStrings.at(role);
    else
        return QString();
}


ModelNode::ModelNode()
: modelCache(0), objectCache(0), isArray(false)
{
}

ModelNode::~ModelNode()
{
    qDeleteAll(properties.values());

    ModelNode *node;
    for (int ii = 0; ii < values.count(); ++ii) {
        node = qvariant_cast<ModelNode *>(values.at(ii));
        if (node) { delete node; node = 0; }
    }

    if (modelCache) { modelCache->m_nested->_root = 0/* ==this */; delete modelCache; modelCache = 0; }
    if (objectCache) { delete objectCache; objectCache = 0; }
}

void ModelNode::setObjectValue(const QScriptValue& valuemap) {
    QScriptValueIterator it(valuemap);
    while (it.hasNext()) {
        it.next();
        ModelNode *value = new ModelNode;
        QScriptValue v = it.value();
        if (v.isArray()) {
            value->isArray = true;
            value->setListValue(v);
        } else {
            value->values << v.toVariant();
            if (objectCache)
                objectCache->setValue(it.name().toUtf8(), value->values.last());
        }
        if (properties.contains(it.name()))
            delete properties[it.name()];
        properties.insert(it.name(), value);
    }
}

void ModelNode::setListValue(const QScriptValue& valuelist) {
    QScriptValueIterator it(valuelist);
    values.clear();
    int size = valuelist.property(QLatin1String("length")).toInt32();
    for (int i=0; i<size; i++) {
        ModelNode *value = new ModelNode;
        QScriptValue v = valuelist.property(i);
        if (v.isArray()) {
            value->isArray = true;
            value->setListValue(v);
        } else if (v.isObject()) {
            value->setObjectValue(v);
        } else {
            value->values << v.toVariant();
        }
        values.append(qVariantFromValue(value));
    }
}

void ModelNode::setProperty(const QString& prop, const QVariant& val) {
    QHash<QString, ModelNode *>::const_iterator it = properties.find(prop);
    if (it != properties.end()) {
        (*it)->values[0] = val;
    } else {
        ModelNode *n = new ModelNode;
        n->values << val;
        properties.insert(prop,n);
    }
    if (objectCache)
        objectCache->setValue(prop.toUtf8(), val);
}

void ModelNode::dump(ModelNode *node, int ind)
{
    QByteArray indentBa(ind * 4, ' ');
    const char *indent = indentBa.constData();

    for (int ii = 0; ii < node->values.count(); ++ii) {
        ModelNode *subNode = qvariant_cast<ModelNode *>(node->values.at(ii));
        if (subNode) {
            qWarning().nospace() << indent << "Sub-node " << ii;
            dump(subNode, ind + 1);
        } else {
            qWarning().nospace() << indent << "Sub-node " << ii << ": " << node->values.at(ii).toString();
        }
    }

    for (QHash<QString, ModelNode *>::ConstIterator iter = node->properties.begin(); iter != node->properties.end(); ++iter) {
        qWarning().nospace() << indent << "Property " << iter.key() << ':';
        dump(iter.value(), ind + 1);
    }
}

ModelObject::ModelObject()
: _mo(new QDeclarativeOpenMetaObject(this))
{
}

void ModelObject::setValue(const QByteArray &name, const QVariant &val)
{
    _mo->setValue(name, val);
    setProperty(name.constData(), val);
}


QT_END_NAMESPACE
