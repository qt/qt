/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qobject.h>
#include <QtDeclarative/qmlengine.h>
#include <private/qmlengine_p.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptengine.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtCore/qxmlstream.h>
#include <private/qmlrefcount_p.h>
#include <private/qmlengine_p.h>
#include <QtCore/qstack.h>
#include "qmlxmlhttprequest_p.h"

#include <QtCore/qdebug.h>

// ### Find real values
#define INVALID_STATE_ERR ((QScriptContext::Error)15)

#define D(arg) (arg)->release()
#define A(arg) (arg)->addref()

namespace {

class NodeImpl : public QmlRefCount
{
public:
    NodeImpl() : type(Element), parent(0) {}
    virtual ~NodeImpl() { 
        if (parent) D(parent); 
        for (int ii = 0; ii < children.count(); ++ii)
            D(children.at(ii));
        for (int ii = 0; ii < attributes.count(); ++ii)
            D(attributes.at(ii));
    }

    // These numbers are copied from the Node IDL definition
    enum Type { 
        Attr = 2, 
        CDATA = 4, 
        Comment = 8, 
        Document = 9, 
        DocumentFragment = 11, 
        DocumentType = 10,
        Element = 1, 
        Entity = 6, 
        EntityReference = 5,
        Notation = 12, 
        ProcessingInstruction = 7, 
        Text = 3
    };
    Type type;

    QString namespaceUri;
    QString name;

    QString data;

    NodeImpl *parent;

    QList<NodeImpl *> children;
    QList<NodeImpl *> attributes;
};

class DocumentImpl : public QmlRefCount
{
public:
    DocumentImpl() : root(0) {}
    virtual ~DocumentImpl() {
        if (root) D(root);
    }

    QString version;
    QString encoding;
    bool isStandalone;

    NodeImpl *root;
};

class NamedNodeMap
{
public:
    // JS API
    static QScriptValue length(QScriptContext *context, QScriptEngine *engine);

    // C++ API
    static QScriptValue prototype(QScriptEngine *);
    static QScriptValue create(QScriptEngine *, NodeImpl *, QList<NodeImpl *> *);

    NamedNodeMap();
    NamedNodeMap(const NamedNodeMap &);
    NamedNodeMap &operator=(const NamedNodeMap &);
    ~NamedNodeMap();
    bool isNull();

    NodeImpl *d;
    QList<NodeImpl *> *list;
};

class NamedNodeMapClass : public QScriptClass
{
public:
    NamedNodeMapClass(QScriptEngine *engine) : QScriptClass(engine) {}

    virtual QueryFlags queryProperty(const QScriptValue &object, const QScriptString &name, QueryFlags flags, uint *id);
    virtual QScriptValue property(const QScriptValue &object, const QScriptString &name, uint id);
};

class NodeList 
{
public:
    // JS API
    static QScriptValue length(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue item(QScriptContext *context, QScriptEngine *engine);

    // C++ API
    static QScriptValue prototype(QScriptEngine *);
    static QScriptValue create(QScriptEngine *, NodeImpl *);

    NodeList();
    NodeList(const NodeList &);
    NodeList &operator=(const NodeList &);
    ~NodeList();
    bool isNull();

    NodeImpl *d;
};

class NodeListClass : public QScriptClass
{
public:
    NodeListClass(QScriptEngine *engine) : QScriptClass(engine) {}
    virtual QueryFlags queryProperty(const QScriptValue &object, const QScriptString &name, QueryFlags flags, uint *id);
    virtual QScriptValue property(const QScriptValue &object, const QScriptString &name, uint id);
};

class Node
{
public:
    // JS API
    static QScriptValue nodeName(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue nodeValue(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue nodeType(QScriptContext *context, QScriptEngine *engine);

    static QScriptValue parentNode(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue childNodes(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue firstChild(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue lastChild(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue previousSibling(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue nextSibling(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue attributes(QScriptContext *context, QScriptEngine *engine);

    //static QScriptValue ownerDocument(QScriptContext *context, QScriptEngine *engine);
    //static QScriptValue namespaceURI(QScriptContext *context, QScriptEngine *engine);
    //static QScriptValue prefix(QScriptContext *context, QScriptEngine *engine);
    //static QScriptValue localName(QScriptContext *context, QScriptEngine *engine);
    //static QScriptValue baseURI(QScriptContext *context, QScriptEngine *engine);
    //static QScriptValue textContent(QScriptContext *context, QScriptEngine *engine);

    // C++ API
    static QScriptValue prototype(QScriptEngine *);
    static QScriptValue create(QScriptEngine *, NodeImpl *);

    Node();
    Node(const Node &o);
    Node &operator=(const Node &);
    ~Node();
    bool isNull() const;

    NodeImpl *d;
};

class Element : public Node
{
public:
    static QScriptValue prototype(QScriptEngine *);
};

class CharacterData : public Node
{
public:
    // JS API
    static QScriptValue length(QScriptContext *context, QScriptEngine *engine);

    // C++ API
    static QScriptValue prototype(QScriptEngine *);
};

class Text : public CharacterData
{
public:
    // JS API
    static QScriptValue isElementContentWhitespace(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue wholeText(QScriptContext *context, QScriptEngine *engine);

    // C++ API
    static QScriptValue prototype(QScriptEngine *);
};

class CDATA : public Text
{
public:
    // C++ API
    static QScriptValue prototype(QScriptEngine *);
};

class Document : public Node
{
public:
    // JS API
    static QScriptValue xmlVersion(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue xmlEncoding(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue xmlStandalone(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue documentElement(QScriptContext *context, QScriptEngine *engine);

    // C++ API
    static QScriptValue prototype(QScriptEngine *);
    static QScriptValue load(QScriptEngine *engine, const QString &data);

    Document();
    Document(const Document &);
    Document &operator=(const Document &);
    ~Document();
    bool isNull() const;

    DocumentImpl *d;
private:
    Document(DocumentImpl *);
};

};

Q_DECLARE_METATYPE(Node);
Q_DECLARE_METATYPE(NodeList);
Q_DECLARE_METATYPE(NamedNodeMap);
Q_DECLARE_METATYPE(Document);

QScriptValue Node::nodeName(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();
    return QScriptValue(node.d->name);
}

QScriptValue Node::nodeValue(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    return QScriptValue(node.d->data);
}

QScriptValue Node::nodeType(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();
    return QScriptValue(node.d->type);
}

QScriptValue Node::parentNode(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    if (node.d->parent) return Node::create(engine, node.d->parent);
    else return engine->nullValue();
}

QScriptValue Node::childNodes(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    return NodeList::create(engine, node.d);
}

QScriptValue Node::firstChild(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    if (node.d->children.isEmpty()) return engine->nullValue();
    else return Node::create(engine, node.d->children.first());
}

QScriptValue Node::lastChild(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    if (node.d->children.isEmpty()) return engine->nullValue();
    else return Node::create(engine, node.d->children.last());
}

QScriptValue Node::previousSibling(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    if (!node.d->parent) return engine->nullValue();

    for (int ii = 0; ii < node.d->parent->children.count(); ++ii) {
        if (node.d->parent->children.at(ii) == node.d) {
            if (ii == 0) return engine->nullValue();
            else return Node::create(engine, node.d->parent->children.at(ii - 1));
        }
    }

    return engine->nullValue();
}

QScriptValue Node::nextSibling(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    if (!node.d->parent) return engine->nullValue();

    for (int ii = 0; ii < node.d->parent->children.count(); ++ii) {
        if (node.d->parent->children.at(ii) == node.d) {
            if ((ii + 1) == node.d->parent->children.count()) return engine->nullValue();
            else return Node::create(engine, node.d->parent->children.at(ii + 1)); 
        }
    }

    return engine->nullValue();
}

QScriptValue Node::attributes(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    if (node.d->type != NodeImpl::Element)
        return engine->nullValue();
    else
        return NamedNodeMap::create(engine, node.d, &node.d->attributes);
}

QScriptValue Node::prototype(QScriptEngine *engine)
{
    QScriptValue proto = engine->newObject();

    proto.setProperty(QLatin1String("nodeName"), engine->newFunction(nodeName), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("nodeValue"), engine->newFunction(nodeValue), QScriptValue::ReadOnly | QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
    proto.setProperty(QLatin1String("nodeType"), engine->newFunction(nodeType), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("parentNode"), engine->newFunction(parentNode), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("childNodes"), engine->newFunction(childNodes), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("firstChild"), engine->newFunction(firstChild), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("lastChild"), engine->newFunction(lastChild), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("previousSibling"), engine->newFunction(previousSibling), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("nextSibling"), engine->newFunction(nextSibling), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("attributes"), engine->newFunction(attributes), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);

    return proto;
}

QScriptValue Node::create(QScriptEngine *engine, NodeImpl *data)
{
    QScriptValue instance = engine->newObject();

    switch (data->type) {
    case NodeImpl::Attr:
    case NodeImpl::Comment:
    case NodeImpl::Document:
    case NodeImpl::DocumentFragment:
    case NodeImpl::DocumentType:
    case NodeImpl::Entity:
    case NodeImpl::EntityReference:
    case NodeImpl::Notation:
    case NodeImpl::CDATA:
        instance.setPrototype(CDATA::prototype(engine));
        break;
    case NodeImpl::ProcessingInstruction:
        instance.setPrototype(Node::prototype(engine));
        break;
    case NodeImpl::Text:
        instance.setPrototype(Text::prototype(engine));
        break;
    case NodeImpl::Element:
        instance.setPrototype(Element::prototype(engine));
        break;
    }

    Node node;
    node.d = data;
    if (data) A(data);

    return engine->newVariant(instance, qVariantFromValue(node));
}

QScriptValue Element::prototype(QScriptEngine *engine)
{
    QScriptValue proto = engine->newObject();
    proto.setPrototype(Node::prototype(engine));

    proto.setProperty(QLatin1String("tagName"), engine->newFunction(nodeName), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);

    return proto;
}

QScriptValue CharacterData::length(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    return QScriptValue(node.d->data.length());
}

QScriptValue CharacterData::prototype(QScriptEngine *engine)
{
    QScriptValue proto = engine->newObject();
    proto.setPrototype(Node::prototype(engine));

    proto.setProperty(QLatin1String("data"), engine->newFunction(nodeValue), QScriptValue::ReadOnly | QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
    proto.setProperty(QLatin1String("length"), engine->newFunction(length), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);

    return proto;
}

QScriptValue Text::isElementContentWhitespace(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    // ### implement
    return QScriptValue(false);
}

QScriptValue Text::wholeText(QScriptContext *context, QScriptEngine *engine)
{
    Node node = qscriptvalue_cast<Node>(context->thisObject());
    if (node.isNull()) return engine->undefinedValue();

    // ### implement
    return QScriptValue(QString());
}

QScriptValue Text::prototype(QScriptEngine *engine)
{
    QScriptValue proto = engine->newObject();
    proto.setPrototype(CharacterData::prototype(engine));

    proto.setProperty(QLatin1String("isElementContentWhitespace"), engine->newFunction(isElementContentWhitespace), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("wholeText"), engine->newFunction(wholeText), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);

    return proto;
}

QScriptValue CDATA::prototype(QScriptEngine *engine)
{
    QScriptValue proto = engine->newObject();
    proto.setPrototype(Text::prototype(engine));
    return proto;
}

QScriptValue Document::prototype(QScriptEngine *engine)
{
    QScriptValue proto = engine->newObject();
    proto.setPrototype(Node::prototype(engine));

    proto.setProperty(QLatin1String("xmlVersion"), engine->newFunction(xmlVersion), QScriptValue::ReadOnly | QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
    proto.setProperty(QLatin1String("xmlEncoding"), engine->newFunction(xmlEncoding), QScriptValue::ReadOnly | QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
    proto.setProperty(QLatin1String("xmlStandalone"), engine->newFunction(xmlStandalone), QScriptValue::ReadOnly | QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
    proto.setProperty(QLatin1String("documentElement"), engine->newFunction(documentElement), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);

    return proto;
}

QScriptValue Document::load(QScriptEngine *engine, const QString &data)
{
    Q_ASSERT(engine);

    DocumentImpl *document = 0;
    QStack<NodeImpl *> nodeStack;

    QXmlStreamReader reader(data);

    while (!reader.atEnd()) {
        switch (reader.readNext()) {
        case QXmlStreamReader::NoToken:
            break;
        case QXmlStreamReader::Invalid:
            break;
        case QXmlStreamReader::StartDocument:
            Q_ASSERT(!document);
            document = new DocumentImpl;
            document->version = reader.documentVersion().toString();
            document->encoding = reader.documentEncoding().toString();
            document->isStandalone = reader.isStandaloneDocument();
            break;
        case QXmlStreamReader::EndDocument:
            break;
        case QXmlStreamReader::StartElement: 
        {
            Q_ASSERT(document);
            NodeImpl *node = new NodeImpl;
            node->namespaceUri = reader.namespaceUri().toString();
            node->name = reader.name().toString();
            if (nodeStack.isEmpty()) {
                document->root = node;
            } else {
                node->parent = nodeStack.top();
                A(node->parent);
                node->parent->children.append(node);
            }
            nodeStack.append(node);

            foreach (const QXmlStreamAttribute &a, reader.attributes()) {
                NodeImpl *attr = new NodeImpl;
                attr->type = NodeImpl::Attr;
                attr->namespaceUri = a.namespaceUri().toString();
                attr->name = a.name().toString();
                attr->data = a.value().toString();
                node->attributes.append(attr);
            }
        } 
            break;
        case QXmlStreamReader::EndElement:
            nodeStack.pop();
            break;
        case QXmlStreamReader::Characters:
        {
            NodeImpl *node = new NodeImpl;
            node->type = reader.isCDATA()?NodeImpl::CDATA:NodeImpl::Text;
            node->parent = nodeStack.top();
            A(node->parent);
            node->parent->children.append(node);
            node->data = reader.text().toString();
        }
            break;
        case QXmlStreamReader::Comment:
            break;
        case QXmlStreamReader::DTD:
            break;
        case QXmlStreamReader::EntityReference:
            break;
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }

    if (!document || reader.hasError()) {
        if (document) D(document);
        return engine->nullValue();
    }

    QScriptValue instance = engine->newObject();
    instance.setPrototype(Document::prototype(engine));
    return engine->newVariant(instance, qVariantFromValue(Document(document)));
}

Node::Node()
: d(0)
{
}

Node::Node(const Node &o)
: d(o.d)
{
    if (d) A(d);
}

Node &Node::operator=(const Node &o)
{
    if (o.d) A(o.d);
    if (d) D(d);
    d = o.d;
    return *this;
}

Node::~Node()
{
    if (d) D(d);
}

bool Node::isNull() const
{
    return d == 0;
}

QScriptValue NamedNodeMap::length(QScriptContext *context, QScriptEngine *engine)
{
    NamedNodeMap map = qscriptvalue_cast<NamedNodeMap>(context->thisObject().data());
    if (map.isNull()) return engine->undefinedValue();

    return QScriptValue(map.list->count());
}

QScriptValue NamedNodeMap::prototype(QScriptEngine *engine)
{
    QScriptValue proto = engine->newObject();

    proto.setProperty(QLatin1String("length"), engine->newFunction(length), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);

    return proto;
}

QScriptValue NamedNodeMap::create(QScriptEngine *engine, NodeImpl *data, QList<NodeImpl *> *list)
{
    QScriptValue instance = engine->newObject();
    instance.setPrototype(NamedNodeMap::prototype(engine));

    NamedNodeMap map;
    map.d = data;
    map.list = list;
    if (data) A(data);

    instance.setData(engine->newVariant(qVariantFromValue(map)));

    if (!QmlEnginePrivate::get(engine)->namedNodeMapClass)
        QmlEnginePrivate::get(engine)->namedNodeMapClass= new NamedNodeMapClass(engine);

    instance.setScriptClass(QmlEnginePrivate::get(engine)->namedNodeMapClass);

    return instance;
}

NamedNodeMap::NamedNodeMap()
: d(0), list(0)
{
}

NamedNodeMap::NamedNodeMap(const NamedNodeMap &o)
: d(o.d), list(o.list)
{
    if (d) A(d);
}

NamedNodeMap &NamedNodeMap::operator=(const NamedNodeMap &o)
{
    if (o.d) A(o.d);
    if (d) D(d);
    d = o.d;
    list = o.list;
    return *this;
}

NamedNodeMap::~NamedNodeMap()
{
    if (d) D(d);
}

bool NamedNodeMap::isNull()
{
    return d == 0;
}

QScriptValue NodeList::item(QScriptContext *context, QScriptEngine *engine)
{
    NodeList list = qscriptvalue_cast<NodeList>(context->thisObject().data());
    if (list.isNull() || context->argumentCount() != 1)
        return engine->undefinedValue();

    qint32 index = context->argument(0).toInt32();

    if (index >= list.d->children.count())
        return engine->undefinedValue(); // ### Exception
    else
        return Node::create(engine, list.d->children.at(index));
}

QScriptValue NodeList::length(QScriptContext *context, QScriptEngine *engine)
{
    NodeList list = qscriptvalue_cast<NodeList>(context->thisObject().data());
    if (list.isNull()) return engine->undefinedValue();

    return QScriptValue(list.d->children.count());
}

QScriptValue NodeList::prototype(QScriptEngine *engine)
{
    QScriptValue proto = engine->newObject();

    proto.setProperty(QLatin1String("length"), engine->newFunction(length), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    proto.setProperty(QLatin1String("item"), engine->newFunction(item, 1), QScriptValue::ReadOnly);

    return proto;
}

QScriptValue NodeList::create(QScriptEngine *engine, NodeImpl *data)
{
    QScriptValue instance = engine->newObject();
    instance.setPrototype(NodeList::prototype(engine));

    NodeList list;
    list.d = data;
    if (data) A(data);

    instance.setData(engine->newVariant(qVariantFromValue(list)));

    if (!QmlEnginePrivate::get(engine)->nodeListClass)
        QmlEnginePrivate::get(engine)->nodeListClass= new NodeListClass(engine);

    instance.setScriptClass(QmlEnginePrivate::get(engine)->nodeListClass);

    return instance;
}

NodeList::NodeList()
: d(0)
{
}

NodeList::NodeList(const NodeList &o)
: d(o.d)
{
    if (d) A(d);
}

NodeList &NodeList::operator=(const NodeList &o)
{
    if (o.d) A(o.d);
    if (d) D(d);
    d = o.d;
    return *this;
}

NodeList::~NodeList()
{
    if (d) D(d);
}

bool NodeList::isNull()
{
    return d == 0;
}

NamedNodeMapClass::QueryFlags NamedNodeMapClass::queryProperty(const QScriptValue &object, const QScriptString &name, QueryFlags flags, uint *id)
{
    if (!(flags & HandlesReadAccess))
        return 0;

    bool ok = false;
    uint index = name.toString().toUInt(&ok);
    if (!ok)
        return 0;

    NamedNodeMap map = qscriptvalue_cast<NamedNodeMap>(object.data());
    if (map.isNull() || (uint)map.list->count() <= index)
        return 0; // ### I think we're meant to raise an exception

    *id = index;
    return HandlesReadAccess;
}

QScriptValue NamedNodeMapClass::property(const QScriptValue &object, const QScriptString &, uint id)
{
    NamedNodeMap map = qscriptvalue_cast<NamedNodeMap>(object.data());
    return Node::create(engine(), map.list->at(id));
}

NodeListClass::QueryFlags NodeListClass::queryProperty(const QScriptValue &object, const QScriptString &name, QueryFlags flags, uint *id)
{
    if (!(flags & HandlesReadAccess))
        return 0;

    bool ok = false;
    uint index = name.toString().toUInt(&ok);
    if (!ok)
        return 0;

    NodeList list = qscriptvalue_cast<NodeList>(object.data());
    if (list.isNull() || (uint)list.d->children.count() <= index)
        return 0; // ### I think we're meant to raise an exception

    *id = index;
    return HandlesReadAccess;
}

QScriptValue NodeListClass::property(const QScriptValue &object, const QScriptString &, uint id)
{
    NodeList list = qscriptvalue_cast<NodeList>(object.data());
    return Node::create(engine(), list.d->children.at(id));
}

Document::Document()
: d(0)
{
}

Document::Document(DocumentImpl *data)
: d(data)
{
}

Document::Document(const Document &o)
: Node(o), d(o.d)
{
    if (d) A(d);
}

Document &Document::operator=(const Document &o)
{
    if (o.d) A(o.d);
    if (d) D(d);
    d = o.d;
    return *this;
}

Document::~Document()
{
    if (d) D(d);
}

bool Document::isNull() const
{
    return d == 0;
}

QScriptValue Document::documentElement(QScriptContext *context, QScriptEngine *engine)
{
    Document document = qscriptvalue_cast<Document>(context->thisObject());
    if (document.isNull()) return engine->undefinedValue();

    if (!document.d->root) return engine->nullValue();

    return Node::create(engine, document.d->root);
}

QScriptValue Document::xmlStandalone(QScriptContext *context, QScriptEngine *engine)
{
    Document document = qscriptvalue_cast<Document>(context->thisObject());
    if (document.isNull()) return engine->undefinedValue();

    if (context->argumentCount())
        document.d->isStandalone = context->argument(0).toBool();

    return QScriptValue(document.d->isStandalone);
}

QScriptValue Document::xmlVersion(QScriptContext *context, QScriptEngine *engine)
{
    Document document = qscriptvalue_cast<Document>(context->thisObject());
    if (document.isNull()) return engine->undefinedValue();

    if (context->argumentCount())
        document.d->version = context->argument(0).toString();

    return QScriptValue(document.d->version);
}

QScriptValue Document::xmlEncoding(QScriptContext *context, QScriptEngine *engine)
{
    Document document = qscriptvalue_cast<Document>(context->thisObject());
    if (document.isNull()) return engine->undefinedValue();

    if (context->argumentCount())
        document.d->encoding = context->argument(0).toString();

    return QScriptValue(document.d->encoding);
}

class QmlXMLHttpRequest : public QObject
{
Q_OBJECT
public:
    enum State { Unsent = 0, 
                 Opened = 1, HeadersReceived = 2,
                 Loading = 3, Done = 4 };

    QmlXMLHttpRequest(QmlEngine *engine);
    virtual ~QmlXMLHttpRequest();

    QScriptValue callback() const;
    void setCallback(const QScriptValue &);

    bool sendFlag() const;
    bool errorFlag() const;
    quint32 readyState() const;
    int replyStatus() const;
    QString replyStatusText() const;

    void open(const QString &, const QUrl &);
    void addHeader(const QString &, const QString &);
    QString header(const QString &name);
    QString headers();
    void send(const QByteArray &);
    void abort();

    QString responseBody() const;
private slots:
    void downloadProgress(qint64);
    void error(QNetworkReply::NetworkError);
    void finished();

private:
    QmlEngine *m_engine;

    State m_state;
    bool m_errorFlag;
    bool m_sendFlag;
    QString m_method;
    QUrl m_url;
    QByteArray m_responseEntityBody;

    typedef QPair<QByteArray, QByteArray> HeaderPair;
    typedef QList<HeaderPair> HeadersList;
    HeadersList m_headersList;
    void fillHeadersList();

    void dispatchCallback();
    QScriptValue m_callback;

    int m_status;
    QString m_statusText;
    QNetworkRequest m_request;
    QNetworkReply *m_network;
    void destroyNetwork();
};

QmlXMLHttpRequest::QmlXMLHttpRequest(QmlEngine *engine)
: m_engine(engine), m_state(Unsent), m_errorFlag(false), m_sendFlag(false),
  m_network(0)
{
    Q_ASSERT(m_engine);
}

QmlXMLHttpRequest::~QmlXMLHttpRequest()
{
    destroyNetwork();
}

QScriptValue QmlXMLHttpRequest::callback() const
{
    return m_callback;
}

void QmlXMLHttpRequest::setCallback(const QScriptValue &c)
{
    m_callback = c;
}

bool QmlXMLHttpRequest::sendFlag() const
{
    return m_sendFlag;
}

bool QmlXMLHttpRequest::errorFlag() const
{
    return m_errorFlag;
}

quint32 QmlXMLHttpRequest::readyState() const
{
    return m_state;
}

int QmlXMLHttpRequest::replyStatus() const
{
    return m_status;
}

QString QmlXMLHttpRequest::replyStatusText() const
{
    return m_statusText;
}

void QmlXMLHttpRequest::open(const QString &method, const QUrl &url)
{
    destroyNetwork();
    m_sendFlag = false;
    m_errorFlag = false;
    m_responseEntityBody = QByteArray();
    m_method = method;
    m_url = url;
    m_state = Opened;
    dispatchCallback();
}

void QmlXMLHttpRequest::addHeader(const QString &name, const QString &value)
{
    QByteArray utfname = name.toUtf8();

    if (m_request.hasRawHeader(utfname)) {
        m_request.setRawHeader(utfname, m_request.rawHeader(utfname) + "," + value.toUtf8());
    } else {
        m_request.setRawHeader(utfname, value.toUtf8());
    }
}

QString QmlXMLHttpRequest::header(const QString &name)
{
    QByteArray utfname = name.toUtf8();

    foreach (const HeaderPair &header, m_headersList) {
        if (header.first == utfname)
            return QString::fromUtf8(header.second);
    }
    return QString();
}

QString QmlXMLHttpRequest::headers()
{
    QString ret;

    foreach (const HeaderPair &header, m_headersList) {
        if (ret.length())
            ret.append(QString::fromUtf8("\r\n"));
        ret.append(QString::fromUtf8(header.first));
        ret.append(QString::fromUtf8(": "));
        ret.append(QString::fromUtf8(header.second));
    }
    return ret;
}

void QmlXMLHttpRequest::fillHeadersList()
{
    QList<QByteArray> headerList = m_network->rawHeaderList();

    m_headersList.clear();
    foreach (const QByteArray &header, headerList) {
        HeaderPair pair (header, m_network->rawHeader(header));
        m_headersList << pair;
    }
}

void QmlXMLHttpRequest::send(const QByteArray &data)
{
    m_errorFlag = false;
    m_sendFlag = true;

    dispatchCallback();

    m_request.setUrl(m_url);

    if (m_method == QLatin1String("GET"))
        m_network = m_engine->networkAccessManager()->get(m_request);
    else if (m_method == QLatin1String("HEAD"))
        m_network = m_engine->networkAccessManager()->head(m_request);
    else if(m_method == QLatin1String("POST"))
        m_network = m_engine->networkAccessManager()->post(m_request, data);
    else if(m_method == QLatin1String("PUT"))
        m_network = m_engine->networkAccessManager()->put(m_request, data);

    QObject::connect(m_network, SIGNAL(downloadProgress(qint64,qint64)), 
                     this, SLOT(downloadProgress(qint64)));
    QObject::connect(m_network, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(error(QNetworkReply::NetworkError)));
    QObject::connect(m_network, SIGNAL(finished()),
                     this, SLOT(finished()));
}

void QmlXMLHttpRequest::abort()
{
    destroyNetwork();
    m_responseEntityBody = QByteArray();
    m_errorFlag = true;
    m_request = QNetworkRequest();

    if (!(m_state == Unsent || 
          (m_state == Opened && !m_sendFlag) ||
          m_state == Done)) {

        m_state = Done;
        m_sendFlag = false;
        dispatchCallback();
    }

    m_state = Unsent;
}

void QmlXMLHttpRequest::downloadProgress(qint64 bytes)
{
    Q_UNUSED(bytes)
    m_status = 
        m_network->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_statusText =
        QString::fromUtf8(m_network->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray());

    // ### We assume if this is called the headers are now available
    if (m_state < HeadersReceived) {
        m_state = HeadersReceived;
        fillHeadersList ();
        dispatchCallback();
    }

    bool wasEmpty = m_responseEntityBody.isEmpty();
    m_responseEntityBody.append(m_network->readAll());
    if (wasEmpty && !m_responseEntityBody.isEmpty()) {
        m_state = Loading;
        dispatchCallback();
    }
}

void QmlXMLHttpRequest::error(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    m_status =
        m_network->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_statusText =
        QString::fromUtf8(m_network->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray());

    m_responseEntityBody = QByteArray();
    m_errorFlag = true;
    m_request = QNetworkRequest();

    destroyNetwork();

    m_state = Done;
    dispatchCallback();
}

void QmlXMLHttpRequest::finished()
{
    // ### We need to transparently redirect as dictated by the spec

    m_status =
        m_network->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_statusText =
        QString::fromUtf8(m_network->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray());

    if (m_state < HeadersReceived) {
        m_state = HeadersReceived;
        fillHeadersList ();
        dispatchCallback();
    }
    m_responseEntityBody.append(m_network->readAll());
    destroyNetwork();
    if (m_state < Loading) {
        m_state = Loading;
        dispatchCallback();
    }
    m_state = Done;
    dispatchCallback();
}


QString QmlXMLHttpRequest::responseBody() const
{
    return QString::fromUtf8(m_responseEntityBody);
}

void QmlXMLHttpRequest::dispatchCallback()
{
    m_callback.call();
}

void QmlXMLHttpRequest::destroyNetwork()
{
    if (m_network) {
        m_network->disconnect();
        m_network->deleteLater();
        m_network = 0;
    }
}

// XMLHttpRequest methods
static QScriptValue qmlxmlhttprequest_open(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (context->argumentCount() < 2 || context->argumentCount() > 5)
        return context->throwError(QScriptContext::SyntaxError, QLatin1String("Incorrect argument count"));

    // Argument 0 - Method
    QString method = context->argument(0).toString().toUpper();
    if (method != QLatin1String("GET") && 
        method != QLatin1String("PUT") &&
        method != QLatin1String("HEAD") &&
        method != QLatin1String("POST"))
        return context->throwError(QScriptContext::SyntaxError, QLatin1String("Unsupported method"));


    // Argument 1 - URL
    QUrl url(context->argument(1).toString()); // ### Need to resolve correctly

    if (url.isRelative()) {
        QmlContext *ctxt = QmlEnginePrivate::get(engine)->currentExpression?QmlEnginePrivate::get(engine)->currentExpression->context():0;
        if (ctxt)
            url = ctxt->resolvedUrl(url);
        else
            return context->throwError(QScriptContext::SyntaxError, QLatin1String("Relative URLs not supported"));
    }

    // Argument 2 - async (optional)
    if (context->argumentCount() > 2 && !context->argument(2).toBoolean())
        return context->throwError(QScriptContext::SyntaxError, QLatin1String("Synchronous call not supported"));


    // Argument 3/4 - user/pass (optional)
    QString username, password;
    if (context->argumentCount() > 3)
        username = context->argument(3).toString();
    if (context->argumentCount() > 4)
        password = context->argument(4).toString();


    // Clear the fragment (if any)
    url.setFragment(QString());
    // Set username/password
    if (!username.isNull()) url.setUserName(username);
    if (!password.isNull()) url.setPassword(password);

    request->open(method, url);

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_setRequestHeader(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (context->argumentCount() != 2)
        return context->throwError(QScriptContext::SyntaxError, QLatin1String("Incorrect argument count"));


    if (request->readyState() != QmlXMLHttpRequest::Opened ||
        request->sendFlag())
        return context->throwError(INVALID_STATE_ERR, QLatin1String("Invalid state"));


    QString name = context->argument(0).toString();
    QString value = context->argument(1).toString();

    // ### Check that name and value are well formed

    QString nameUpper = name.toUpper();
    if (nameUpper == QLatin1String("ACCEPT-CHARSET") ||
        nameUpper == QLatin1String("ACCEPT-ENCODING") ||
        nameUpper == QLatin1String("CONNECTION") ||
        nameUpper == QLatin1String("CONTENT-LENGTH") ||
        nameUpper == QLatin1String("COOKIE") ||
        nameUpper == QLatin1String("COOKIE2") ||
        nameUpper == QLatin1String("CONTENT-TRANSFER-ENCODING") ||
        nameUpper == QLatin1String("DATE") ||
        nameUpper == QLatin1String("EXPECT") ||
        nameUpper == QLatin1String("HOST") ||
        nameUpper == QLatin1String("KEEP-ALIVE") ||
        nameUpper == QLatin1String("REFERER") ||
        nameUpper == QLatin1String("TE") ||
        nameUpper == QLatin1String("TRAILER") ||
        nameUpper == QLatin1String("TRANSFER-ENCODING") ||
        nameUpper == QLatin1String("UPGRADE") ||
        nameUpper == QLatin1String("USER-AGENT") ||
        nameUpper == QLatin1String("VIA") ||
        nameUpper.startsWith(QLatin1String("PROXY-")) ||
        nameUpper.startsWith(QLatin1String("SEC-"))) 
        return engine->undefinedValue();

    request->addHeader(nameUpper, value);

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_send(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() != QmlXMLHttpRequest::Opened)
        return context->throwError(INVALID_STATE_ERR, QLatin1String("Invalid state"));

    if (request->sendFlag())
        return context->throwError(INVALID_STATE_ERR, QLatin1String("Invalid state"));

    QByteArray data;
    if (context->argumentCount() > 0)
        data = context->argument(0).toString().toUtf8();

    request->send(data);

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_abort(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    request->abort();

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_getResponseHeader(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (context->argumentCount() != 1)
        return context->throwError(QScriptContext::SyntaxError, QLatin1String("Incorrect argument count"));

    if (request->readyState() != QmlXMLHttpRequest::Loading &&
        request->readyState() != QmlXMLHttpRequest::Done &&
        request->readyState() != QmlXMLHttpRequest::HeadersReceived)
        return context->throwError(INVALID_STATE_ERR, QLatin1String("Invalid state"));

    QString headerName = context->argument(0).toString();

    return QScriptValue(request->header(headerName));
}

static QScriptValue qmlxmlhttprequest_getAllResponseHeaders(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (context->argumentCount() != 0)
        return context->throwError(QScriptContext::SyntaxError, QLatin1String("Incorrect argument count"));

    if (request->readyState() != QmlXMLHttpRequest::Loading &&
        request->readyState() != QmlXMLHttpRequest::Done &&
        request->readyState() != QmlXMLHttpRequest::HeadersReceived)
        return context->throwError(INVALID_STATE_ERR, QLatin1String("Invalid state"));

    return QScriptValue(request->headers());
}

// XMLHttpRequest properties
static QScriptValue qmlxmlhttprequest_readyState(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    return QScriptValue(request->readyState());
}

static QScriptValue qmlxmlhttprequest_status(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() == QmlXMLHttpRequest::Unsent ||
        request->readyState() == QmlXMLHttpRequest::Opened)
        return context->throwError(INVALID_STATE_ERR, QLatin1String("Invalid state"));

    if (request->errorFlag())
        return QScriptValue(0);
    else
        return QScriptValue(request->replyStatus());
}

static QScriptValue qmlxmlhttprequest_statusText(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() == QmlXMLHttpRequest::Unsent ||
        request->readyState() == QmlXMLHttpRequest::Opened)
        return context->throwError(INVALID_STATE_ERR, QLatin1String("Invalid state"));

    if (request->errorFlag())
        return QScriptValue(0);
    else
        return QScriptValue(request->replyStatusText());
}

static QScriptValue qmlxmlhttprequest_responseText(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() != QmlXMLHttpRequest::Loading &&
        request->readyState() != QmlXMLHttpRequest::Done)
        return QScriptValue(QString());
    else
        return QScriptValue(request->responseBody());
}

static QScriptValue qmlxmlhttprequest_responseXML(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() != QmlXMLHttpRequest::Loading &&
        request->readyState() != QmlXMLHttpRequest::Done)
        return engine->nullValue();
    else  
        return Document::load(engine, request->responseBody());
}

static QScriptValue qmlxmlhttprequest_onreadystatechange(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (context->argumentCount())
        request->setCallback(context->argument(0));

    return request->callback();
}

// Constructor
static QScriptValue qmlxmlhttprequest_new(QScriptContext *context, QScriptEngine *engine)
{
    if (context->isCalledAsConstructor()) {
        context->thisObject().setData(engine->newQObject(new QmlXMLHttpRequest(QmlEnginePrivate::getEngine(engine)), QScriptEngine::ScriptOwnership));
    }
    return engine->undefinedValue();
}

void qt_add_qmlxmlhttprequest(QScriptEngine *engine)
{
    QScriptValue prototype = engine->newObject();

    // Methods
    prototype.setProperty(QLatin1String("open"), engine->newFunction(qmlxmlhttprequest_open, 2));
    prototype.setProperty(QLatin1String("setRequestHeader"), engine->newFunction(qmlxmlhttprequest_setRequestHeader, 2));
    prototype.setProperty(QLatin1String("send"), engine->newFunction(qmlxmlhttprequest_send));
    prototype.setProperty(QLatin1String("abort"), engine->newFunction(qmlxmlhttprequest_abort));
    prototype.setProperty(QLatin1String("getResponseHeader"), engine->newFunction(qmlxmlhttprequest_getResponseHeader, 1));
    prototype.setProperty(QLatin1String("getAllResponseHeaders"), engine->newFunction(qmlxmlhttprequest_getAllResponseHeaders));

    // Read-only properties
    prototype.setProperty(QLatin1String("readyState"), engine->newFunction(qmlxmlhttprequest_readyState), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("status"), engine->newFunction(qmlxmlhttprequest_status), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("statusText"), engine->newFunction(qmlxmlhttprequest_statusText), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("responseText"), engine->newFunction(qmlxmlhttprequest_responseText), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("responseXML"), engine->newFunction(qmlxmlhttprequest_responseXML), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("onreadystatechange"), engine->newFunction(qmlxmlhttprequest_onreadystatechange), QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

    // Constructor
    QScriptValue constructor = engine->newFunction(qmlxmlhttprequest_new, prototype);
    constructor.setProperty(QLatin1String("UNSENT"), 0, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    constructor.setProperty(QLatin1String("OPENED"), 1, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    constructor.setProperty(QLatin1String("HEADERS_RECEIVED"), 2, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    constructor.setProperty(QLatin1String("LOADING"), 3, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    constructor.setProperty(QLatin1String("DONE"), 4, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    engine->globalObject().setProperty(QLatin1String("XMLHttpRequest"), constructor);
}

#include "qmlxmlhttprequest.moc"
