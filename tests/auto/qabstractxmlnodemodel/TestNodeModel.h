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


#include <QtXmlPatterns/QAbstractXmlNodeModel>

/*!
 \class TestNodeModel
 \since 4.4
 \internal
 \brief Subclass of QAbstractXmlNodeModel, used by tst_QAbstractXmlNodeModel, for testing only.
 */
class TestNodeModel : public QAbstractXmlNodeModel
{
public:
    virtual QUrl baseUri(const QXmlNodeModelIndex&) const;
    virtual QUrl documentUri(const QXmlNodeModelIndex&) const;
    virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex&) const;
    virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex&, const QXmlNodeModelIndex&) const;
    virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex&) const;
    virtual QXmlName name(const QXmlNodeModelIndex&) const;
    virtual QString stringValue(const QXmlNodeModelIndex&) const;
    virtual QVariant typedValue(const QXmlNodeModelIndex&) const;
    virtual QVector<QXmlName> namespaceBindings(const QXmlNodeModelIndex&) const;
    virtual QXmlNodeModelIndex elementById(const QXmlName &ncname) const;
    virtual QVector<QXmlNodeModelIndex> nodesByIdref(const QXmlName &ncname) const;

protected:
    virtual QXmlNodeModelIndex nextFromSimpleAxis(SimpleAxis axis,
                                                  const QXmlNodeModelIndex &origin) const;
    virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex&) const;

};

QUrl TestNodeModel::baseUri(const QXmlNodeModelIndex&) const
{
    return QUrl();
}

QUrl TestNodeModel::documentUri(const QXmlNodeModelIndex&) const
{
    return QUrl();
}

QXmlNodeModelIndex::NodeKind TestNodeModel::kind(const QXmlNodeModelIndex&) const
{
    return QXmlNodeModelIndex::Element;
}

QXmlNodeModelIndex::DocumentOrder TestNodeModel::compareOrder(const QXmlNodeModelIndex&, const QXmlNodeModelIndex&) const
{
    return QXmlNodeModelIndex::Precedes;
}

QXmlNodeModelIndex TestNodeModel::root(const QXmlNodeModelIndex&) const
{
    return QXmlNodeModelIndex();
}

QXmlName TestNodeModel::name(const QXmlNodeModelIndex&) const
{
    return QXmlName();
}

QString TestNodeModel::stringValue(const QXmlNodeModelIndex&) const
{
    return QString();
}

QVariant TestNodeModel::typedValue(const QXmlNodeModelIndex&) const
{
    return QVariant();
}

QVector<QXmlName> TestNodeModel::namespaceBindings(const QXmlNodeModelIndex&) const
{
    return QVector<QXmlName>();
}

QXmlNodeModelIndex TestNodeModel::elementById(const QXmlName &ncname) const
{
    Q_UNUSED(ncname);
    return QXmlNodeModelIndex();
}

QVector<QXmlNodeModelIndex> TestNodeModel::nodesByIdref(const QXmlName &ncname) const
{
    Q_UNUSED(ncname);
    return QVector<QXmlNodeModelIndex>();
}

QXmlNodeModelIndex TestNodeModel::nextFromSimpleAxis(SimpleAxis, const QXmlNodeModelIndex &) const
{
    return QXmlNodeModelIndex();
}

QVector<QXmlNodeModelIndex> TestNodeModel::attributes(const QXmlNodeModelIndex&) const
{
    return QVector<QXmlNodeModelIndex>();
}

