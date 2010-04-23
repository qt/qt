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

#ifndef GRAPHICSLAYOUTS_H
#define GRAPHICSLAYOUTS_H

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsGridLayout>
#include <qdeclarative.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QGraphicsLinearLayoutStretchItemObject : public QObject, public QGraphicsLayoutItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayoutItem)
public:
    QGraphicsLinearLayoutStretchItemObject(QObject *parent = 0);

    virtual QSizeF sizeHint(Qt::SizeHint, const QSizeF &) const;
};

class LinearLayoutAttached;
class QGraphicsLinearLayoutObject : public QObject, public QGraphicsLinearLayout
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayout QGraphicsLayoutItem)

    Q_PROPERTY(QDeclarativeListProperty<QGraphicsLayoutItem> children READ children)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsLinearLayoutObject(QObject * = 0);
    ~QGraphicsLinearLayoutObject();

    QDeclarativeListProperty<QGraphicsLayoutItem> children() { return QDeclarativeListProperty<QGraphicsLayoutItem>(this, 0, children_append, children_count, children_at, children_clear); }

    static LinearLayoutAttached *qmlAttachedProperties(QObject *);

private Q_SLOTS:
    void updateStretch(QGraphicsLayoutItem*,int);
    void updateAlignment(QGraphicsLayoutItem*,Qt::Alignment);

private:
    friend class LinearLayoutAttached;
    void clearChildren();
    void insertLayoutItem(int, QGraphicsLayoutItem *);
    static QHash<QGraphicsLayoutItem*, LinearLayoutAttached*> attachedProperties;

    static void children_append(QDeclarativeListProperty<QGraphicsLayoutItem> *prop, QGraphicsLayoutItem *item) {
        static_cast<QGraphicsLinearLayoutObject*>(prop->object)->insertLayoutItem(-1, item);
    }

    static void children_clear(QDeclarativeListProperty<QGraphicsLayoutItem> *prop) {
        static_cast<QGraphicsLinearLayoutObject*>(prop->object)->clearChildren();
    }

    static int children_count(QDeclarativeListProperty<QGraphicsLayoutItem> *prop) {
        return static_cast<QGraphicsLinearLayoutObject*>(prop->object)->count();
    }

    static QGraphicsLayoutItem *children_at(QDeclarativeListProperty<QGraphicsLayoutItem> *prop, int index) {
        return static_cast<QGraphicsLinearLayoutObject*>(prop->object)->itemAt(index);
    }
};

class GridLayoutAttached;
class QGraphicsGridLayoutObject : public QObject, public QGraphicsGridLayout
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayout QGraphicsLayoutItem)

    Q_PROPERTY(QDeclarativeListProperty<QGraphicsLayoutItem> children READ children)
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(qreal verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)
    Q_PROPERTY(qreal horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsGridLayoutObject(QObject * = 0);
    ~QGraphicsGridLayoutObject();

    QDeclarativeListProperty<QGraphicsLayoutItem> children() { return QDeclarativeListProperty<QGraphicsLayoutItem>(this, 0, children_append, children_count, children_at, children_clear); }

    qreal spacing() const;

    static GridLayoutAttached *qmlAttachedProperties(QObject *);

private:
    friend class GraphicsLayoutAttached;
    void addWidget(QGraphicsWidget *);
    void clearChildren();
    void addLayoutItem(QGraphicsLayoutItem *);
    static QHash<QGraphicsLayoutItem*, GridLayoutAttached*> attachedProperties;

    static void children_append(QDeclarativeListProperty<QGraphicsLayoutItem> *prop, QGraphicsLayoutItem *item) {
        static_cast<QGraphicsGridLayoutObject*>(prop->object)->addLayoutItem(item);
    }

    static void children_clear(QDeclarativeListProperty<QGraphicsLayoutItem> *prop) {
        static_cast<QGraphicsGridLayoutObject*>(prop->object)->clearChildren();
    }

    static int children_count(QDeclarativeListProperty<QGraphicsLayoutItem> *prop) {
        return static_cast<QGraphicsGridLayoutObject*>(prop->object)->count();
    }

    static QGraphicsLayoutItem *children_at(QDeclarativeListProperty<QGraphicsLayoutItem> *prop, int index) {
        return static_cast<QGraphicsGridLayoutObject*>(prop->object)->itemAt(index);
    }
};

class LinearLayoutAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int stretchFactor READ stretchFactor WRITE setStretchFactor NOTIFY stretchChanged)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
public:
    LinearLayoutAttached(QObject *parent);

    int stretchFactor() const { return _stretch; }
    void setStretchFactor(int f);
    Qt::Alignment alignment() const { return _alignment; }
    void setAlignment(Qt::Alignment a);

Q_SIGNALS:
    void stretchChanged(QGraphicsLayoutItem*,int);
    void alignmentChanged(QGraphicsLayoutItem*,Qt::Alignment);

private:
    int _stretch;
    Qt::Alignment _alignment;
};

class GridLayoutAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int row READ row WRITE setRow)
    Q_PROPERTY(int column READ column WRITE setColumn)
    Q_PROPERTY(int rowSpan READ rowSpan WRITE setRowSpan)
    Q_PROPERTY(int columnSpan READ columnSpan WRITE setColumnSpan)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
public:
    GridLayoutAttached(QObject *parent);

    int row() const { return _row; }
    void setRow(int r);

    int column() const { return _column; }
    void setColumn(int c);

    int rowSpan() const { return _rowspan; }
    void setRowSpan(int rs);

    int columnSpan() const { return _colspan; }
    void setColumnSpan(int cs);

    Qt::Alignment alignment() const { return _alignment; }
    void setAlignment(Qt::Alignment a);

private:
    int _row;
    int _column;
    int _rowspan;
    int _colspan;
    Qt::Alignment _alignment;
};

QT_END_NAMESPACE

QML_DECLARE_INTERFACE(QGraphicsLayoutItem)
QML_DECLARE_INTERFACE(QGraphicsLayout)
QML_DECLARE_TYPE(QGraphicsLinearLayoutStretchItemObject)
QML_DECLARE_TYPE(QGraphicsLinearLayoutObject)
QML_DECLARE_TYPEINFO(QGraphicsLinearLayoutObject, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QGraphicsGridLayoutObject)
QML_DECLARE_TYPEINFO(QGraphicsGridLayoutObject, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif // GRAPHICSLAYOUTS_H
