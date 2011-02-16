/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  node.cpp
*/

#include "node.h"
#include "tree.h"
#include "codemarker.h"
#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \class Node
  \brief The Node class is a node in the Tree.

  A Node represents a class or function or something else
  from the source code..
 */

/*!
  When this Node is destroyed, if it has a parent Node, it
  removes itself from the parent node's child list.
 */
Node::~Node()
{
    if (par)
        par->removeChild(this);
    if (rel)
        rel->removeRelated(this);
}

/*!
  Sets this Node's Doc to \a doc. If \a replace is false and
  this Node already has a Doc, a warning is reported that the
  Doc is being overridden, and it reports where the previous
  Doc was found. If \a replace is true, the Doc is replaced
  silently.
 */
void Node::setDoc(const Doc& doc, bool replace)
{
    if (!d.isEmpty() && !replace) {
        doc.location().warning(tr("Overrides a previous doc"));
        d.location().warning(tr("(The previous doc is here)"));
    }
    d = doc;
}

/*!
  Construct a node with the given \a type and having the
  given \a parent and \a name. The new node is added to the
  parent's child list.
 */
Node::Node(Type type, InnerNode *parent, const QString& name)
    : typ(type),
      acc(Public),
      saf(UnspecifiedSafeness),
      pageTyp(NoPageType),
      sta(Commendable),
      par(parent),
      rel(0),
      nam(name)
{
    if (par)
        par->addChild(this);
    //uuid = QUuid::createUuid();
}

/*!
  Returns the node's URL.
 */
QString Node::url() const
{
    return u;
}

/*!
  Sets the node's URL to \a url
 */
void Node::setUrl(const QString &url)
{
    u = url;
}

void Node::setPageType(const QString& t)
{
    if ((t == "API") || (t == "api"))
        pageTyp = ApiPage;
    else if (t == "article")
        pageTyp = ArticlePage;
    else if (t == "example")
        pageTyp = ExamplePage;
}

/*!
  Sets the pointer to the node that this node relates to.
 */
void Node::setRelates(InnerNode *pseudoParent)
{
    if (rel)
            rel->removeRelated(this);
    rel = pseudoParent;
    pseudoParent->related.append(this);
}

/*!
  This function creates a pair that describes a link.
  The pair is composed from \a link and \a desc. The
  \a linkType is the map index the pair is filed under.
 */
void Node::setLink(LinkType linkType, const QString &link, const QString &desc)
{
    QPair<QString,QString> linkPair;
    linkPair.first = link;
    linkPair.second = desc;
    linkMap[linkType] = linkPair;
}

/*!
  Returns a string representing the access specifier.
 */
QString Node::accessString() const
{
    switch (acc) {
    case Protected:
        return "protected";
    case Private:
        return "private";
    case Public:
    default:
        break;
    }
    return "public";
}

/*!
  Extract a class name from the type \a string and return it.
 */
QString Node::extractClassName(const QString &string) const
{
    QString result;
    for (int i=0; i<=string.size(); ++i) {
        QChar ch;
        if (i != string.size())
            ch = string.at(i);

        QChar lower = ch.toLower();
        if ((lower >= QLatin1Char('a') && lower <= QLatin1Char('z')) ||
            ch.digitValue() >= 0 ||
            ch == QLatin1Char('_') ||
            ch == QLatin1Char(':')) {
            result += ch;
        }
        else if (!result.isEmpty()) {
            if (result != QLatin1String("const"))
                return result;
            result.clear();
        }
    }
    return result;
}

/*!
  Returns a string representing the access specifier.
 */
QString RelatedClass::accessString() const
{
    switch (access) {
    case Node::Protected:
        return "protected";
    case Node::Private:
        return "private";
    case Node::Public:
    default:
        break;
    }
    return "public";
}

/*!
 */
Node::Status Node::inheritedStatus() const
{
    Status parentStatus = Commendable;
    if (par)
        parentStatus = par->inheritedStatus();
    return (Status)qMin((int)sta, (int)parentStatus);
}

/*!
  Returns the thread safeness value for whatever this node
  represents. But if this node has a parent and the thread
  safeness value of the parent is the same as the thread
  safeness value of this node, what is returned is the
  value \c{UnspecifiedSafeness}. Why?
 */
Node::ThreadSafeness Node::threadSafeness() const
{
    if (par && saf == par->inheritedThreadSafeness())
        return UnspecifiedSafeness;
    return saf;
}

/*!
  If this node has a parent, the parent's thread safeness
  value is returned. Otherwise, this node's thread safeness
  value is returned. Why?
 */
Node::ThreadSafeness Node::inheritedThreadSafeness() const
{
    if (par && saf == UnspecifiedSafeness)
        return par->inheritedThreadSafeness();
    return saf;
}

/*!
  Returns the sanitized file name without the path.
  If the the file is an html file, the html suffix
  is removed. Why?
 */
QString Node::fileBase() const
{
    QString base = name();
    if (base.endsWith(".html"))
        base.chop(5);
    base.replace(QRegExp("[^A-Za-z0-9]+"), " ");
    base = base.trimmed();
    base.replace(" ", "-");
    return base.toLower();
}

/*!
  Returns this node's Universally Unique IDentifier.
  If its UUID has not yet been created, it is created
  first.
 */
QUuid Node::guid() const
{
    if (uuid.isNull())
        uuid = QUuid::createUuid();
    return uuid;
}

/*!
  Composes a string to be used as an href attribute in DITA
  XML. It is composed of the file name and the UUID separated
  by a '#'. If this node is a class node, the file name is
  taken from this node; if this node is a function node, the
  file name is taken from the parent node of this node.
 */
QString Node::ditaXmlHref()
{
    QString href;
    if ((type() == Function) ||
        (type() == Property) ||
        (type() == Variable)) {
        href = parent()->fileBase();
    }
    else {
        href = fileBase();
    }
    if (!href.endsWith(".xml"))
        href += ".xml";
    return href + "#" + guid();
}

/*!
  \class InnerNode
 */

/*!
  The inner node destructor deletes the children and removes
  this node from its related nodes.
 */
InnerNode::~InnerNode()
{
    deleteChildren();
    removeFromRelated();
}

/*!
  Find the node in this node's children that has the
  given \a name. If this node is a QML class node, be
  sure to also look in the children of its property
  group nodes. Return the matching node or 0.
 */
Node *InnerNode::findNode(const QString& name)
{
    Node *node = childMap.value(name);
    if (node && node->subType() != QmlPropertyGroup)
        return node;
    if ((type() == Fake) && (subType() == QmlClass)) {
        for (int i=0; i<children.size(); ++i) {
            Node* n = children.at(i);
            if (n->subType() == QmlPropertyGroup) {
                node = static_cast<InnerNode*>(n)->findNode(name);
                if (node)
                    return node;
            }
        }
    }
    return primaryFunctionMap.value(name);
}

/*!
  Same as the other findNode(), but if the node with the
  specified \a name is not of the specified \a type, return
  0.
 */
Node *InnerNode::findNode(const QString& name, Type type)
{
    if (type == Function) {
        return primaryFunctionMap.value(name);
    }
    else {
        Node *node = childMap.value(name);
        if (node && node->type() == type) {
            return node;
        }
        else {
            return 0;
        }
    }
}

/*!
  Find the function node in this node for the function named \a name.
 */
FunctionNode *InnerNode::findFunctionNode(const QString& name)
{
    return static_cast<FunctionNode *>(primaryFunctionMap.value(name));
}

/*!
  Find the function node in this node that has the same name as \a clone.
 */
FunctionNode *InnerNode::findFunctionNode(const FunctionNode *clone)
{
    QMap<QString, Node *>::ConstIterator c =
	    primaryFunctionMap.find(clone->name());
    if (c != primaryFunctionMap.end()) {
	if (isSameSignature(clone, (FunctionNode *) *c)) {
	    return (FunctionNode *) *c;
	}
        else if (secondaryFunctionMap.contains(clone->name())) {
	    const NodeList& secs = secondaryFunctionMap[clone->name()];
	    NodeList::ConstIterator s = secs.begin();
	    while (s != secs.end()) {
		if (isSameSignature(clone, (FunctionNode *) *s))
		    return (FunctionNode *) *s;
		++s;
	    }
	}
    }
    return 0;
}

/*!
  Returns the list of keys from the primary function map.
 */
QStringList InnerNode::primaryKeys()
{
    QStringList t;
    QMap<QString, Node*>::iterator i = primaryFunctionMap.begin();
    while (i != primaryFunctionMap.end()) {
        t.append(i.key());
        ++i;
    }
    return t;
}

/*!
  Returns the list of keys from the secondary function map.
 */
QStringList InnerNode::secondaryKeys()
{
    QStringList t;
    QMap<QString, NodeList>::iterator i = secondaryFunctionMap.begin();
    while (i != secondaryFunctionMap.end()) {
        t.append(i.key());
        ++i;
    }
    return t;
}

/*!
 */
void InnerNode::setOverload(const FunctionNode *func, bool overlode)
{
    Node *node = (Node *) func;
    Node *&primary = primaryFunctionMap[func->name()];

    if (secondaryFunctionMap.contains(func->name())) {
	NodeList& secs = secondaryFunctionMap[func->name()];
	if (overlode) {
	    if (primary == node) {
		primary = secs.first();
		secs.erase(secs.begin());
		secs.append(node);
	    }
            else {
		secs.removeAll(node);
                secs.append(node);
            }
	}
        else {
	    if (primary != node) {
		secs.removeAll(node);
		secs.prepend(primary);
		primary = node;
	    }
	}
    }
}

/*!
  Mark all child nodes that have no documentation as having
  private access and internal status. qdoc will then ignore
  them for documentation purposes.
 */
void InnerNode::makeUndocumentedChildrenInternal()
{
    foreach (Node *child, childNodes()) {
        if (child->doc().isEmpty()) {
            child->setAccess(Node::Private);
            child->setStatus(Node::Internal);
        }
    }
}

/*!
 */
void InnerNode::normalizeOverloads()
{
    QMap<QString, Node *>::Iterator p1 = primaryFunctionMap.begin();
    while (p1 != primaryFunctionMap.end()) {
        FunctionNode *primaryFunc = (FunctionNode *) *p1;
        if (secondaryFunctionMap.contains(primaryFunc->name()) &&
            (primaryFunc->status() != Commendable ||
             primaryFunc->access() == Private)) {

	    NodeList& secs = secondaryFunctionMap[primaryFunc->name()];
	    NodeList::ConstIterator s = secs.begin();
	    while (s != secs.end()) {
		FunctionNode *secondaryFunc = (FunctionNode *) *s;

                // Any non-obsolete, non-compatibility, non-private functions
                // (i.e, visible functions) are preferable to the primary
                // function.

                if (secondaryFunc->status() == Commendable &&
                    secondaryFunc->access() != Private) {

                    *p1 = secondaryFunc;
                    int index = secondaryFunctionMap[primaryFunc->name()].indexOf(secondaryFunc);
                    secondaryFunctionMap[primaryFunc->name()].replace(index, primaryFunc);
                    break;
                }
                ++s;
            }
        }
        ++p1;
    }

    QMap<QString, Node *>::ConstIterator p = primaryFunctionMap.begin();
    while (p != primaryFunctionMap.end()) {
	FunctionNode *primaryFunc = (FunctionNode *) *p;
	if (primaryFunc->isOverload())
	    primaryFunc->ove = false;
	if (secondaryFunctionMap.contains(primaryFunc->name())) {
	    NodeList& secs = secondaryFunctionMap[primaryFunc->name()];
	    NodeList::ConstIterator s = secs.begin();
	    while (s != secs.end()) {
		FunctionNode *secondaryFunc = (FunctionNode *) *s;
		if (!secondaryFunc->isOverload())
		    secondaryFunc->ove = true;
		++s;
	    }
	}
	++p;
    }

    NodeList::ConstIterator c = childNodes().begin();
    while (c != childNodes().end()) {
	if ((*c)->isInnerNode())
	    ((InnerNode *) *c)->normalizeOverloads();
	++c;
    }
}

/*!
 */
void InnerNode::removeFromRelated()
{
    while (!related.isEmpty()) {
        Node *p = static_cast<Node *>(related.takeFirst());

        if (p != 0 && p->relates() == this) p->clearRelated();
    }
}

/*!
 */
void InnerNode::deleteChildren()
{
    qDeleteAll(children);
}

/*!
  Returns true because this is an inner node.
 */
bool InnerNode::isInnerNode() const
{
    return true;
}

/*!
 */
const Node *InnerNode::findNode(const QString& name) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findNode(name);
}

/*!
 */
const Node *InnerNode::findNode(const QString& name, Type type) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findNode(name, type);
}

/*!
  Find the function node in this node that has the given \a name.
 */
const FunctionNode *InnerNode::findFunctionNode(const QString& name) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findFunctionNode(name);
}

/*!
  Find the function node in this node that has the same name as \a clone.
 */
const FunctionNode *InnerNode::findFunctionNode(const FunctionNode *clone) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findFunctionNode(clone);
}

/*!
 */
const EnumNode *InnerNode::findEnumNodeForValue(const QString &enumValue) const
{
    foreach (const Node *node, enumChildren) {
        const EnumNode *enume = static_cast<const EnumNode *>(node);
        if (enume->hasItem(enumValue))
            return enume;
    }
    return 0;
}

/*!
  Returnds the sequence number of the function node \a func
  in the list of overloaded functions for a class, such that
  all the functions have the same name as the \a func.
 */
int InnerNode::overloadNumber(const FunctionNode *func) const
{
    Node *node = (Node *) func;
    if (primaryFunctionMap[func->name()] == node) {
        return 1;
    }
    else {
        return secondaryFunctionMap[func->name()].indexOf(node) + 2;
    }
}

/*!
  Returns the number of member functions of a class such that
  the functions are all named \a funcName.
 */
int InnerNode::numOverloads(const QString& funcName) const
{
    if (primaryFunctionMap.contains(funcName)) {
        return secondaryFunctionMap[funcName].count() + 1;
    }
    else {
        return 0;
    }
}

/*!
  Returns a node list containing all the member functions of
  some class such that the functions overload the name \a funcName.
 */
NodeList InnerNode::overloads(const QString &funcName) const
{
    NodeList result;
    Node *primary = primaryFunctionMap.value(funcName);
    if (primary) {
        result << primary;
        result += secondaryFunctionMap[funcName];
    }
    return result;
}

/*!
  Construct an inner node (i.e., not a leaf node) of the
  given \a type and having the given \a parent and \a name.
 */
InnerNode::InnerNode(Type type, InnerNode *parent, const QString& name)
    : Node(type, parent, name)
{
    switch (type) {
    case Class:
    case Namespace:
        setPageType(ApiPage);
        break;
    default:
        break;
    }
}

/*!
  Appends an \a include file to the list of include files.
 */
void InnerNode::addInclude(const QString& include)
{
    inc.append(include);
}

/*!
  Sets the list of include files to \a includes.
 */
void InnerNode::setIncludes(const QStringList& includes)
{
    inc = includes;
}

/*!
  f1 is always the clone
 */
bool InnerNode::isSameSignature(const FunctionNode *f1, const FunctionNode *f2)
{
    if (f1->parameters().count() != f2->parameters().count())
        return false;
    if (f1->isConst() != f2->isConst())
        return false;

    QList<Parameter>::ConstIterator p1 = f1->parameters().begin();
    QList<Parameter>::ConstIterator p2 = f2->parameters().begin();
    while (p2 != f2->parameters().end()) {
	if ((*p1).hasType() && (*p2).hasType()) {
            if ((*p1).rightType() != (*p2).rightType())
                return false;

            QString t1 = p1->leftType();
            QString t2 = p2->leftType();

            if (t1.length() < t2.length())
                qSwap(t1, t2);

            /*
              ### hack for C++ to handle superfluous
              "Foo::" prefixes gracefully
            */
            if (t1 != t2 && t1 != (f2->parent()->name() + "::" + t2))
                return false;
        }
        ++p1;
        ++p2;
    }
    return true;
}

/*!
  Adds the \a child to this node's child list.
 */
void InnerNode::addChild(Node *child)
{
    children.append(child);
    if ((child->type() == Function) || (child->type() == QmlMethod)) {
        FunctionNode *func = (FunctionNode *) child;
        if (!primaryFunctionMap.contains(func->name())) {
            primaryFunctionMap.insert(func->name(), func);
	}
        else {
	    NodeList &secs = secondaryFunctionMap[func->name()];
	    secs.append(func);
	}
    }
    else {
        if (child->type() == Enum)
            enumChildren.append(child);
        childMap.insert(child->name(), child);
    }
}

/*!
 */
void InnerNode::removeChild(Node *child)
{
    children.removeAll(child);
    enumChildren.removeAll(child);
    if (child->type() == Function) {
	QMap<QString, Node *>::Iterator prim =
		primaryFunctionMap.find(child->name());
	NodeList& secs = secondaryFunctionMap[child->name()];
	if (prim != primaryFunctionMap.end() && *prim == child) {
	    if (secs.isEmpty()) {
		primaryFunctionMap.remove(child->name());
	    }
            else {
		primaryFunctionMap.insert(child->name(), secs.takeFirst());
	    }
	}
        else {
	    secs.removeAll(child);
	}
        QMap<QString, Node *>::Iterator ent = childMap.find( child->name() );
        if (ent != childMap.end() && *ent == child)
            childMap.erase( ent );
    }
    else {
	QMap<QString, Node *>::Iterator ent = childMap.find(child->name());
	if (ent != childMap.end() && *ent == child)
	    childMap.erase(ent);
    }
}

/*!
  Find the module (QtCore, QtGui, etc.) to which the class belongs.
  We do this by obtaining the full path to the header file's location
  and examine everything between "src/" and the filename.  This is
  semi-dirty because we are assuming a particular directory structure.

  This function is only really useful if the class's module has not
  been defined in the header file with a QT_MODULE macro or with an
  \inmodule command in the documentation.
*/
QString Node::moduleName() const
{
    if (!mod.isEmpty())
        return mod;

    QString path = location().filePath();
    QString pattern = QString("src") + QDir::separator();
    int start = path.lastIndexOf(pattern);

    if (start == -1)
        return "";

    QString moduleDir = path.mid(start + pattern.size());
    int finish = moduleDir.indexOf(QDir::separator());

    if (finish == -1)
        return "";

    QString moduleName = moduleDir.left(finish);

    if (moduleName == "corelib")
        return "QtCore";
    else if (moduleName == "uitools")
        return "QtUiTools";
    else if (moduleName == "gui")
        return "QtGui";
    else if (moduleName == "network")
        return "QtNetwork";
    else if (moduleName == "opengl")
        return "QtOpenGL";
    else if (moduleName == "qt3support")
        return "Qt3Support";
    else if (moduleName == "svg")
        return "QtSvg";
    else if (moduleName == "sql")
        return "QtSql";
    else if (moduleName == "qtestlib")
        return "QtTest";
    else if (moduleDir.contains("webkit"))
        return "QtWebKit";
    else if (moduleName == "xml")
        return "QtXml";
    else
        return "";
}

/*!
 */
void InnerNode::removeRelated(Node *pseudoChild)
{
    related.removeAll(pseudoChild);
}

/*!
  \class LeafNode
 */

/*!
  Returns false because this is a LeafNode.
 */
bool LeafNode::isInnerNode() const
{
    return false;
}

/*!
  Constructs a leaf node named \a name of the specified
  \a type. The new leaf node becomes a child of \a parent.
 */
LeafNode::LeafNode(Type type, InnerNode *parent, const QString& name)
    : Node(type, parent, name)
{
    switch (type) {
    case Enum:
    case Function:
    case Typedef:
    case Variable:
    case QmlProperty:
    case QmlSignal:
    case QmlMethod:
        setPageType(ApiPage);
        break;
    default:
        break;
    }
}

/*!
  \class NamespaceNode
 */

/*!
  Constructs a namespace node.
 */
NamespaceNode::NamespaceNode(InnerNode *parent, const QString& name)
    : InnerNode(Namespace, parent, name)
{
    setPageType(ApiPage);
}

/*!
  \class ClassNode
  \brief This class represents a C++ class.
 */

/*!
  Constructs a class node. A class node will generate an API page.
 */
ClassNode::ClassNode(InnerNode *parent, const QString& name)
    : InnerNode(Class, parent, name)
{
    hidden = false;
    abstract = false;
    setPageType(ApiPage);
}

/*!
 */
void ClassNode::addBaseClass(Access access,
                             ClassNode *node,
                             const QString &dataTypeWithTemplateArgs)
{
    bases.append(RelatedClass(access, node, dataTypeWithTemplateArgs));
    node->derived.append(RelatedClass(access, this));
}

/*!
 */
void ClassNode::fixBaseClasses()
{
    int i;
    i = 0;
    while (i < bases.size()) {
        ClassNode* bc = bases.at(i).node;
        if (bc->access() == Node::Private) {
            RelatedClass rc = bases.at(i);
            bases.removeAt(i);
            ignoredBases.append(rc);
            const QList<RelatedClass> &bb = bc->baseClasses();
            for (int j = bb.size() - 1; j >= 0; --j)
                bases.insert(i, bb.at(j));
        }
        else {
            ++i;
        }
    }

    i = 0;
    while (i < derived.size()) {
        ClassNode* dc = derived.at(i).node;
        if (dc->access() == Node::Private) {
            derived.removeAt(i);
            const QList<RelatedClass> &dd = dc->derivedClasses();
            for (int j = dd.size() - 1; j >= 0; --j)
                derived.insert(i, dd.at(j));
        }
        else {
            ++i;
        }
    }
}

/*!
  Search the child list to find the property node with the
  specified \a name.
 */
const PropertyNode* ClassNode::findPropertyNode(const QString& name) const
{
    const Node* n = findNode(name,Node::Property);
    return (n ? static_cast<const PropertyNode*>(n) : 0);
}

/*!
  \class FakeNode
 */

/*!
  The type of a FakeNode is Fake, and it has a \a subtype,
  which specifies the type of FakeNode. The page type for
  the page index is set here.
 */
FakeNode::FakeNode(InnerNode *parent, const QString& name, SubType subtype)
    : InnerNode(Fake, parent, name), sub(subtype)
{
    switch (subtype) {
    case Module:
    case Page:
    case Group:
        setPageType(ArticlePage);
        break;
    case QmlClass:
    case QmlBasicType:
        setPageType(ApiPage);
        break;
    case Example:
        setPageType(ExamplePage);
        break;
    default:
        break;
    }
}

/*!
  Returns the fake node's title. This is used for the page title.
*/
QString FakeNode::title() const
{
    return tle;
}

/*!
  Returns the fake node's full title, which is usually
  just title(), but for some SubType values is different
  from title()
 */
QString FakeNode::fullTitle() const
{
    if (sub == File) {
        if (title().isEmpty())
            return name().mid(name().lastIndexOf('/') + 1) + " Example File";
        else
            return title();
    }
    else if (sub == Image) {
        if (title().isEmpty())
            return name().mid(name().lastIndexOf('/') + 1) + " Image File";
        else
            return title();
    }
    else if (sub == HeaderFile) {
        if (title().isEmpty())
            return name();
        else
            return name() + " - " + title();
    }
    else {
        return title();
    }
}

/*!
  Returns the subtitle.
 */
QString FakeNode::subTitle() const
{
    if (!stle.isEmpty())
        return stle;

    if ((sub == File) || (sub == Image)) {
        if (title().isEmpty() && name().contains("/"))
            return name();
    }
    return QString();
}

/*!
  \class EnumNode
 */

/*!
  The constructor for the node representing an enum type
  has a \a parent class and an enum type \a name.
 */
EnumNode::EnumNode(InnerNode *parent, const QString& name)
    : LeafNode(Enum, parent, name), ft(0)
{
}

/*!
  Add \a item to the enum type's item list.
 */
void EnumNode::addItem(const EnumItem& item)
{
    itms.append(item);
    names.insert(item.name());
}

/*!
  Returns the access level of the enumeration item named \a name.
  Apparently it is private if it has been omitted by qdoc's
  omitvalue command. Otherwise it is public.
 */
Node::Access EnumNode::itemAccess(const QString &name) const
{
    if (doc().omitEnumItemNames().contains(name))
        return Private;
    return Public;
}

/*!
  Returns the enum value associated with the enum \a name.
 */
QString EnumNode::itemValue(const QString &name) const
{
    foreach (const EnumItem &item, itms) {
        if (item.name() == name)
            return item.value();
    }
    return QString();
}

/*!
  \class TypedefNode
 */

/*!
 */
TypedefNode::TypedefNode(InnerNode *parent, const QString& name)
    : LeafNode(Typedef, parent, name), ae(0)
{
}

/*!
 */
void TypedefNode::setAssociatedEnum(const EnumNode *enume)
{
    ae = enume;
}

/*!
  \class Parameter
  \brief The class Parameter contains one parameter.

  A parameter can be a function parameter or a macro
  parameter.
 */

/*!
  Constructs this parameter from the left and right types
  \a leftType and rightType, the parameter \a name, and the
  \a defaultValue. In practice, \a rightType is not used,
  and I don't know what is was meant for.
 */
Parameter::Parameter(const QString& leftType,
                     const QString& rightType,
		     const QString& name,
                     const QString& defaultValue)
    : lef(leftType), rig(rightType), nam(name), def(defaultValue)
{
}

/*!
  The standard copy constructor copies the strings from \a p.
 */
Parameter::Parameter(const Parameter& p)
    : lef(p.lef), rig(p.rig), nam(p.nam), def(p.def)
{
}

/*!
  Assigning Parameter \a p to this Parameter copies the
  strings across.
 */
Parameter& Parameter::operator=(const Parameter& p)
{
    lef = p.lef;
    rig = p.rig;
    nam = p.nam;
    def = p.def;
    return *this;
}

/*!
  Reconstructs the text describing the parameter and
  returns it. If \a value is true, the default value
  will be included, if there is one.
 */
QString Parameter::reconstruct(bool value) const
{
    QString p = lef + rig;
    if (!p.endsWith(QChar('*')) && !p.endsWith(QChar('&')) && !p.endsWith(QChar(' ')))
        p += " ";
    p += nam;
    if (value)
        p += def;
    return p;
}


/*!
  \class FunctionNode
 */

/*!
  Construct a function node for a C++ function. It's parent
  is \a parent, and it's name is \a name.
 */
FunctionNode::FunctionNode(InnerNode *parent, const QString& name)
    : LeafNode(Function, parent, name),
      met(Plain),
      vir(NonVirtual),
      con(false),
      sta(false),
      ove(false),
      att(false),
      rf(0),
      ap(0)
{
    // nothing.
}

/*!
  Construct a function node for a QML method or signal, specified
  by \a type. It's parent is \a parent, and it's name is \a name.
  If \a attached is true, it is an attached method or signal.
 */
FunctionNode::FunctionNode(Type type, InnerNode *parent, const QString& name, bool attached)
    : LeafNode(type, parent, name),
      met(Plain),
      vir(NonVirtual),
      con(false),
      sta(false),
      ove(false),
      att(attached),
      rf(0),
      ap(0)
{
    // nothing.
}

/*!
  Sets the \a virtualness of this function. If the \a virtualness
  is PureVirtual, and if the parent() is a ClassNode, set the parent's
  \e abstract flag to true.
 */
void FunctionNode::setVirtualness(Virtualness virtualness)
{
    vir = virtualness;
    if ((virtualness == PureVirtual) && parent() &&
        (parent()->type() == Node::Class))
        parent()->setAbstract(true);
}

/*!
 */
void FunctionNode::setOverload(bool overlode)
{
    parent()->setOverload(this, overlode);
    ove = overlode;
}

/*!
  Sets the function node's reimplementation flag to \a r.
  When \a r is true, it is supposed to mean that this function
  is a reimplementation of a virtual function in a base class,
  but it really just means the \e reimp command was seen in the
  qdoc comment.
 */
void FunctionNode::setReimp(bool r)
{
    reimp = r;
}

/*!
 */
void FunctionNode::addParameter(const Parameter& parameter)
{
    params.append(parameter);
}

/*!
 */
void FunctionNode::borrowParameterNames(const FunctionNode *source)
{
    QList<Parameter>::Iterator t = params.begin();
    QList<Parameter>::ConstIterator s = source->params.begin();
    while (s != source->params.end() && t != params.end()) {
	if (!(*s).name().isEmpty())
	    (*t).setName((*s).name());
	++s;
	++t;
    }
}

/*!
  If this function is a reimplementation, \a from points
  to the FunctionNode of the function being reimplemented.
 */
void FunctionNode::setReimplementedFrom(FunctionNode *from)
{
    rf = from;
    from->rb.append(this);
}

/*!
  Sets the "associated" property to \a property. The function
  might be the setter or getter for a property, for example.
 */
void FunctionNode::setAssociatedProperty(PropertyNode *property)
{
    ap = property;
}

/*!
  Returns the overload number for this function obtained
  from the parent.
 */
int FunctionNode::overloadNumber() const
{
    return parent()->overloadNumber(this);
}

/*!
  Returns the number of times this function name has been
  overloaded, obtained from the parent.
 */
int FunctionNode::numOverloads() const
{
    return parent()->numOverloads(name());
}

/*!
  Returns the list of parameter names.
 */
QStringList FunctionNode::parameterNames() const
{
    QStringList names;
    QList<Parameter>::ConstIterator p = parameters().begin();
    while (p != parameters().end()) {
        names << (*p).name();
        ++p;
    }
    return names;
}

/*!
  Returns a raw list of parameters. If \a names is true, the
  names are included. If \a values is true, the default values
  are included, if any are present.
 */
QString FunctionNode::rawParameters(bool names, bool values) const
{
    QString raw;
    foreach (const Parameter &parameter, parameters()) {
        raw += parameter.leftType() + parameter.rightType();
        if (names)
            raw += parameter.name();
        if (values)
            raw += parameter.defaultValue();
    }
    return raw;
}

/*!
  Returns the list of reconstructed parameters. If \a values
  is true, the default values are included, if any are present.
 */
QStringList FunctionNode::reconstructParams(bool values) const
{
    QStringList params;
    QList<Parameter>::ConstIterator p = parameters().begin();
    while (p != parameters().end()) {
        params << (*p).reconstruct(values);
        ++p;
    }
    return params;
}

/*!
  Reconstructs and returns the function's signature. If \a values
  is true, the default values of the parameters are included, if
  present.
 */
QString FunctionNode::signature(bool values) const
{
    QString s;
    if (!returnType().isEmpty())
        s = returnType() + " ";
    s += name() + "(";
    QStringList params = reconstructParams(values);
    int p = params.size();
    if (p > 0) {
        for (int i=0; i<p; i++) {
            s += params[i];
            if (i < (p-1))
                s += ", ";
        }
    }
    s += ")";
    return s;
}

/*!
  Print some debugging stuff.
 */
void FunctionNode::debug() const
{
    qDebug("QML METHOD %s rt %s pp %s",
            qPrintable(name()), qPrintable(rt), qPrintable(pp.join(" ")));
}

/*!
  \class PropertyNode

  This class describes one instance of using the Q_PROPERTY macro.
 */

/*!
  The constructor sets the \a parent and the \a name, but
  everything else is set to default values.
 */
PropertyNode::PropertyNode(InnerNode *parent, const QString& name)
    : LeafNode(Property, parent, name),
      sto(Trool_Default),
      des(Trool_Default),
      scr(Trool_Default),
      wri(Trool_Default),
      usr(Trool_Default),
      cst(false),
      fnl(false),
      overrides(0)
{
    // nothing.
}

/*!
  Sets this property's \e {overridden from} property to
  \a baseProperty, which indicates that this property
  overrides \a baseProperty. To begin with, all the values
  in this property are set to the corresponding values in
  \a baseProperty.

  We probably should ensure that the constant and final
  attributes are not being overridden improperly.
 */
void PropertyNode::setOverriddenFrom(const PropertyNode* baseProperty)
{
    for (int i = 0; i < NumFunctionRoles; ++i) {
        if (funcs[i].isEmpty())
            funcs[i] = baseProperty->funcs[i];
    }
    if (sto == Trool_Default)
        sto = baseProperty->sto;
    if (des == Trool_Default)
        des = baseProperty->des;
    if (scr == Trool_Default)
        scr = baseProperty->scr;
    if (wri == Trool_Default)
        wri = baseProperty->wri;
    if (usr == Trool_Default)
        usr = baseProperty->usr;
    overrides = baseProperty;
}

/*!
 */
QString PropertyNode::qualifiedDataType() const
{
    if (setters().isEmpty() && resetters().isEmpty()) {
        if (dt.contains("*") || dt.contains("&")) {
            // 'QWidget *' becomes 'QWidget *' const
            return dt + " const";
        }
        else {
            /*
              'int' becomes 'const int' ('int const' is
              correct C++, but looks wrong)
            */
            return "const " + dt;
        }
    }
    else {
        return dt;
    }
}

/*! Converts the \a boolean value to an enum representation
  of the boolean type, which includes an enum  value for the
  \e {default value} of the item, i.e. true, false, or default.
 */
PropertyNode::Trool PropertyNode::toTrool(bool boolean)
{
    return boolean ? Trool_True : Trool_False;
}

/*!
  Converts the enum \a troolean back to a boolean value.
  If \a troolean is neither the true enum value nor the
  false enum value, the boolean value returned is
  \a defaultValue.

  Note that runtimeDesignabilityFunction() should be called
  first. If that function returns the name of a function, it
  means the function must be called at runtime to determine
  whether the property is Designable.
 */
bool PropertyNode::fromTrool(Trool troolean, bool defaultValue)
{
    switch (troolean) {
    case Trool_True:
        return true;
    case Trool_False:
        return false;
    default:
        return defaultValue;
    }
}

/*!
  \class TargetNode
 */

/*!
 */
TargetNode::TargetNode(InnerNode *parent, const QString& name)
    : LeafNode(Target, parent, name)
{
}

/*!
  Returns false because this is a TargetNode.
 */
bool TargetNode::isInnerNode() const
{
    return false;
}

#ifdef QDOC_QML
bool QmlClassNode::qmlOnly = false;
QMultiMap<QString,Node*> QmlClassNode::inheritedBy;

/*!
  Constructs a Qml class node (i.e. a Fake node with the
  subtype QmlClass. The new node has the given \a parent
  and \a name and is associated with the C++ class node
  specified by \a cn which may be null if the the Qml
  class node is not associated with a C++ class node.
 */
QmlClassNode::QmlClassNode(InnerNode *parent,
                           const QString& name,
                           const ClassNode* cn)
    : FakeNode(parent, name, QmlClass), cnode(cn)
{
    if (name.startsWith(QLatin1String("QML:")))
        setTitle((qmlOnly ? QLatin1String("") : QLatin1String("QML ")) + name.mid(4) + QLatin1String(" Element"));
    else
        setTitle((qmlOnly ? QLatin1String("") : QLatin1String("QML ")) + name + QLatin1String(" Element"));
}

/*!
  I made this so I could print a debug message here.
 */
QmlClassNode::~QmlClassNode()
{
#ifdef DEBUG_MULTIPLE_QDOCCONF_FILES
    qDebug() << "Deleting QmlClassNode:" << name();
#endif
}

/*!
  Clear the multimap so that subsequent runs don't try to use
  nodes from a previous run.
 */
void QmlClassNode::clear()
{
    inheritedBy.clear();
}

/*!
  The base file name for this kind of node has "qml_"
  prepended to it.

  But not yet. Still testing.
 */
QString QmlClassNode::fileBase() const
{
#if 0
    if (Node::fileBase() == "item")
        qDebug() << "FILEBASE: qmlitem" << name();
    return "qml_" + Node::fileBase();
#endif
    return Node::fileBase();
}

/*!
  Record the fact that QML class \a base is inherited by
  QML class \a sub.
 */
void QmlClassNode::addInheritedBy(const QString& base, Node* sub)
{
    inheritedBy.insert(base,sub);
#ifdef DEBUG_MULTIPLE_QDOCCONF_FILES
    qDebug() << "QmlClassNode::addInheritedBy(): insert" << base << sub->name() << inheritedBy.size();
#endif
}

/*!
  Loads the list \a subs with the nodes of all the subclasses of \a base.
 */
void QmlClassNode::subclasses(const QString& base, NodeList& subs)
{
    subs.clear();
    if (inheritedBy.count(base) > 0) {
        subs = inheritedBy.values(base);
#ifdef DEBUG_MULTIPLE_QDOCCONF_FILES
        qDebug() << "QmlClassNode::subclasses():" <<  inheritedBy.count(base) << base
                 << "subs:" << subs.size() << "total size:" << inheritedBy.size();
#endif
    }
}

/*!
  Constructs a Qml basic type node (i.e. a Fake node with
  the subtype QmlBasicType. The new node has the given
  \a parent and \a name.
 */
QmlBasicTypeNode::QmlBasicTypeNode(InnerNode *parent,
                                   const QString& name)
    : FakeNode(parent, name, QmlBasicType)
{
    setTitle(name);
}

/*!
  Constructor for the Qml property group node. \a parent is
  always a QmlClassNode.
 */
QmlPropGroupNode::QmlPropGroupNode(QmlClassNode* parent,
                                   const QString& name,
                                   bool attached)
    : FakeNode(parent, name, QmlPropertyGroup),
      isdefault(false),
      att(attached)
{
    // nothing.
}

/*!
  Constructor for the QML property node.
 */
QmlPropertyNode::QmlPropertyNode(QmlPropGroupNode *parent,
                                 const QString& name,
                                 const QString& type,
                                 bool attached)
    : LeafNode(QmlProperty, parent, name),
      dt(type),
      sto(Trool_Default),
      des(Trool_Default),
      att(attached)
{
    setPageType(ApiPage);
}

/*!
  I don't know what this is.
 */
QmlPropertyNode::Trool QmlPropertyNode::toTrool(bool boolean)
{
    return boolean ? Trool_True : Trool_False;
}

/*!
  I don't know what this is either.
 */
bool QmlPropertyNode::fromTrool(Trool troolean, bool defaultValue)
{
    switch (troolean) {
    case Trool_True:
        return true;
    case Trool_False:
        return false;
    default:
        return defaultValue;
    }
}

static QString valueType(const QString& n)
{
    if (n == "QPoint")
        return "QDeclarativePointValueType";
    if (n == "QPointF")
        return "QDeclarativePointFValueType";
    if (n == "QSize")
        return "QDeclarativeSizeValueType";
    if (n == "QSizeF")
        return "QDeclarativeSizeFValueType";
    if (n == "QRect")
        return "QDeclarativeRectValueType";
    if (n == "QRectF")
        return "QDeclarativeRectFValueType";
    if (n == "QVector2D")
        return "QDeclarativeVector2DValueType";
    if (n == "QVector3D")
        return "QDeclarativeVector3DValueType";
    if (n == "QVector4D")
        return "QDeclarativeVector4DValueType";
    if (n == "QQuaternion")
        return "QDeclarativeQuaternionValueType";
    if (n == "QMatrix4x4")
        return "QDeclarativeMatrix4x4ValueType";
    if (n == "QEasingCurve")
        return "QDeclarativeEasingValueType";
    if (n == "QFont")
        return "QDeclarativeFontValueType";
    return QString();
}

/*!
  Returns true if a QML property or attached property is
  read-only. The algorithm for figuring this out is long
  amd tedious and almost certainly will break. It currently
  doesn't work for qmlproperty bool PropertyChanges::explicit,
  because the tokenized gets confused on "explicit" .
 */
bool QmlPropertyNode::isWritable(const Tree* tree) const
{
    Node* n = parent();
    while (n && n->subType() != Node::QmlClass)
        n = n->parent();
    if (n) {
        const QmlClassNode* qcn = static_cast<const QmlClassNode*>(n);
        const ClassNode* cn = qcn->classNode();
        if (cn) {
            QStringList dotSplit = name().split(QChar('.'));
            const PropertyNode* pn = cn->findPropertyNode(dotSplit[0]);
            if (pn) {
                if (dotSplit.size() > 1) {
                    QStringList path(extractClassName(pn->qualifiedDataType()));
                    const Node* nn = tree->findNode(path,Class);
                    if (nn) {
                        const ClassNode* cn = static_cast<const ClassNode*>(nn);
                        pn = cn->findPropertyNode(dotSplit[1]);
                        if (pn) {
                            return pn->isWritable();
                        }
                        else {
                            const QList<RelatedClass>& bases = cn->baseClasses();
                            if (!bases.isEmpty()) {
                                for (int i=0; i<bases.size(); ++i) {
                                    const ClassNode* cn = bases[i].node;
                                    pn = cn->findPropertyNode(dotSplit[1]);
                                    if (pn) {
                                        return pn->isWritable();
                                    }
                                }
                            }
                            const QList<RelatedClass>& ignoredBases = cn->ignoredBaseClasses();
                            if (!ignoredBases.isEmpty()) {
                                for (int i=0; i<ignoredBases.size(); ++i) {
                                    const ClassNode* cn = ignoredBases[i].node;
                                    pn = cn->findPropertyNode(dotSplit[1]);
                                    if (pn) {
                                        return pn->isWritable();
                                    }
                                }
                            }
                            QString vt = valueType(cn->name());
                            if (!vt.isEmpty()) {
                                QStringList path(vt);
                                const Node* vtn = tree->findNode(path,Class);
                                if (vtn) {
                                    const ClassNode* cn = static_cast<const ClassNode*>(vtn);
                                    pn = cn->findPropertyNode(dotSplit[1]);
                                    if (pn) {
                                        return pn->isWritable();
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    return pn->isWritable();
                }
            }
            else {
                const QList<RelatedClass>& bases = cn->baseClasses();
                if (!bases.isEmpty()) {
                    for (int i=0; i<bases.size(); ++i) {
                        const ClassNode* cn = bases[i].node;
                        pn = cn->findPropertyNode(dotSplit[0]);
                        if (pn) {
                            return pn->isWritable();
                        }
                    }
                }
                const QList<RelatedClass>& ignoredBases = cn->ignoredBaseClasses();
                if (!ignoredBases.isEmpty()) {
                    for (int i=0; i<ignoredBases.size(); ++i) {
                        const ClassNode* cn = ignoredBases[i].node;
                        pn = cn->findPropertyNode(dotSplit[0]);
                        if (pn) {
                            return pn->isWritable();
                        }
                    }
                }
                if (isAttached()) {
                    QString classNameAttached = cn->name() + "Attached";
                    QStringList path(classNameAttached);
                    const Node* nn = tree->findNode(path,Class);
                    const ClassNode* acn = static_cast<const ClassNode*>(nn);
                    pn = acn->findPropertyNode(dotSplit[0]);
                    if (pn) {
                        return pn->isWritable();
                    }
                }
            }
        }
    }
    location().warning(tr("Can't determine read-only status of QML property %1; writable assumed.").arg(name()));
    return true;
}

#endif

QT_END_NAMESPACE
