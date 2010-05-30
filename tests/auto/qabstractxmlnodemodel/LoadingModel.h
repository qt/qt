/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QSimpleXmlNodeModel>
#include <QVector>

class LoadingModel : public QSimpleXmlNodeModel
{
public:
    virtual ~LoadingModel();
    static QAbstractXmlNodeModel::Ptr create(const QXmlNamePool &np);

    virtual QUrl documentUri(const QXmlNodeModelIndex &) const;
    virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &) const;
    virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex &, const QXmlNodeModelIndex&) const;
    virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &) const;
    virtual QXmlName name(const QXmlNodeModelIndex &) const;
    virtual QVariant typedValue(const QXmlNodeModelIndex &) const;
    virtual QString stringValue(const QXmlNodeModelIndex &) const;
    virtual QXmlNodeModelIndex nextFromSimpleAxis(QAbstractXmlNodeModel::SimpleAxis, const QXmlNodeModelIndex &) const;
    virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex &) const;

private:
    friend class Loader;
    class Node
    {
    public:
        inline Node(const QXmlNodeModelIndex::NodeKind k,
                    const Node *const p,
                    const QString &v = QString(),
                    const QXmlName &n = QXmlName()) : kind(k)
                                                    , value(v)
                                                    , parent(p)
                                                    , precedingSibling(0)
                                                    , followingSibling(0)
                                                    , firstChild(0)
                                                    , name(n)
        {
        }

        typedef QVector<const Node *> Vector;
        QXmlNodeModelIndex::NodeKind    kind;
        QString                         value;
        const Node *                    parent;
        const Node *                    precedingSibling;
        const Node *                    followingSibling;
        const Node *                    firstChild;
        Node::Vector                    attributes;
        QXmlName                        name;
    };

    inline const Node *toInternal(const QXmlNodeModelIndex &ni) const;
    inline QXmlNodeModelIndex createIndex(const Node *const internal) const;

    LoadingModel(const Node::Vector &content,
                 const QXmlNamePool &np);

    Node::Vector m_nodes;
};

