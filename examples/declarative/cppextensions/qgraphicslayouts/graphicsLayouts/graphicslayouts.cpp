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

#include "graphicslayouts_p.h"

#include <QtGui/qgraphicswidget.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

LinearLayoutAttached::LinearLayoutAttached(QObject *parent)
: QObject(parent), _stretch(1), _alignment(Qt::AlignCenter), _spacing(0)
{
}

void LinearLayoutAttached::setStretchFactor(int f)
{
    if (_stretch == f)
        return;

    _stretch = f;
    emit stretchChanged(reinterpret_cast<QGraphicsLayoutItem*>(parent()), _stretch);
}

void LinearLayoutAttached::setSpacing(int s)
{
    if (_spacing == s)
        return;

    _spacing = s;
    emit spacingChanged(reinterpret_cast<QGraphicsLayoutItem*>(parent()), _spacing);
}

void LinearLayoutAttached::setAlignment(Qt::Alignment a)
{
    if (_alignment == a)
        return;

    _alignment = a;
    emit alignmentChanged(reinterpret_cast<QGraphicsLayoutItem*>(parent()), _alignment);
}

QGraphicsLinearLayoutStretchItemObject::QGraphicsLinearLayoutStretchItemObject(QObject *parent)
    : QObject(parent)
{
}

QSizeF QGraphicsLinearLayoutStretchItemObject::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
Q_UNUSED(which);
Q_UNUSED(constraint);
return QSizeF();
}


QGraphicsLinearLayoutObject::QGraphicsLinearLayoutObject(QObject *parent)
: QObject(parent)
{
}

QGraphicsLinearLayoutObject::~QGraphicsLinearLayoutObject()
{
}

void QGraphicsLinearLayoutObject::insertLayoutItem(int index, QGraphicsLayoutItem *item)
{
insertItem(index, item);

//connect attached properties
if (LinearLayoutAttached *obj = attachedProperties.value(item)) {
    setStretchFactor(item, obj->stretchFactor());
    setAlignment(item, obj->alignment());
    updateSpacing(item, obj->spacing());
    QObject::connect(obj, SIGNAL(stretchChanged(QGraphicsLayoutItem*,int)),
                     this, SLOT(updateStretch(QGraphicsLayoutItem*,int)));
    QObject::connect(obj, SIGNAL(alignmentChanged(QGraphicsLayoutItem*,Qt::Alignment)),
                     this, SLOT(updateAlignment(QGraphicsLayoutItem*,Qt::Alignment)));
    QObject::connect(obj, SIGNAL(spacingChanged(QGraphicsLayoutItem*,int)),
                     this, SLOT(updateSpacing(QGraphicsLayoutItem*,int)));
    //### need to disconnect when widget is removed?
}
}

//### is there a better way to do this?
void QGraphicsLinearLayoutObject::clearChildren()
{
for (int i = 0; i < count(); ++i)
    removeAt(i);
}

qreal QGraphicsLinearLayoutObject::contentsMargin() const
{
    qreal a,b,c,d;
    getContentsMargins(&a, &b, &c, &d);
    if(a==b && a==c && a==d)
        return a;
    return -1;
}

void QGraphicsLinearLayoutObject::setContentsMargin(qreal m)
{
    setContentsMargins(m,m,m,m);
}

void QGraphicsLinearLayoutObject::updateStretch(QGraphicsLayoutItem *item, int stretch)
{
QGraphicsLinearLayout::setStretchFactor(item, stretch);
}

void QGraphicsLinearLayoutObject::updateSpacing(QGraphicsLayoutItem* item, int spacing)
{
    for(int i=0; i < count(); i++){
        if(itemAt(i) == item){ //I do not see the reverse function, which is why we must loop over all items
            QGraphicsLinearLayout::setItemSpacing(i, spacing);
            return;
        }
    }
}

void QGraphicsLinearLayoutObject::updateAlignment(QGraphicsLayoutItem *item, Qt::Alignment alignment)
{
QGraphicsLinearLayout::setAlignment(item, alignment);
}

QHash<QGraphicsLayoutItem*, LinearLayoutAttached*> QGraphicsLinearLayoutObject::attachedProperties;
LinearLayoutAttached *QGraphicsLinearLayoutObject::qmlAttachedProperties(QObject *obj)
{
// ### This is not allowed - you must attach to any object
if (!qobject_cast<QGraphicsLayoutItem*>(obj))
    return 0;
LinearLayoutAttached *rv = new LinearLayoutAttached(obj);
attachedProperties.insert(qobject_cast<QGraphicsLayoutItem*>(obj), rv);
return rv;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// QGraphicsGridLayout-related classes
//////////////////////////////////////////////////////////////////////////////////////////////////////
GridLayoutAttached::GridLayoutAttached(QObject *parent)
: QObject(parent), _row(-1), _column(-1), _rowspan(1), _colspan(1), _alignment(-1), _rowstretch(-1),
    _colstretch(-1), _rowspacing(-1), _colspacing(-1), _rowprefheight(-1), _rowmaxheight(-1), _rowminheight(-1),
    _rowfixheight(-1), _colprefwidth(-1), _colmaxwidth(-1), _colminwidth(-1), _colfixwidth(-1)
{
}

void GridLayoutAttached::setRow(int r)
{
    if (_row == r)
        return;

    _row = r;
    //emit rowChanged(reinterpret_cast<QGraphicsLayoutItem*>(parent()), _row);
}

void GridLayoutAttached::setColumn(int c)
{
    if (_column == c)
        return;

    _column = c;
    //emit columnChanged(reinterpret_cast<QGraphicsLayoutItem*>(parent()), _column);
}

void GridLayoutAttached::setRowSpan(int rs)
{
    if (_rowspan == rs)
        return;

    _rowspan = rs;
    //emit rowSpanChanged(reinterpret_cast<QGraphicsLayoutItem*>(parent()), _rowSpan);
}

void GridLayoutAttached::setColumnSpan(int cs)
{
    if (_colspan == cs)
        return;

    _colspan = cs;
    //emit columnSpanChanged(reinterpret_cast<QGraphicsLayoutItem*>(parent()), _columnSpan);
}

void GridLayoutAttached::setAlignment(Qt::Alignment a)
{
    if (_alignment == a)
        return;

    _alignment = a;
    emit alignmentChanged(reinterpret_cast<QGraphicsLayoutItem*>(parent()), _alignment);
}

void GridLayoutAttached::setRowStretchFactor(int f)
{
    _rowstretch = f;
}

void GridLayoutAttached::setColumnStretchFactor(int f)
{
    _colstretch = f;
}

void GridLayoutAttached::setRowSpacing(int s)
{
    _rowspacing = s;
}

void GridLayoutAttached::setColumnSpacing(int s)
{
    _colspacing = s;
}


QGraphicsGridLayoutObject::QGraphicsGridLayoutObject(QObject *parent)
: QObject(parent)
{
}

QGraphicsGridLayoutObject::~QGraphicsGridLayoutObject()
{
}

void QGraphicsGridLayoutObject::addWidget(QGraphicsWidget *wid)
{
//use attached properties
if (QObject *obj = attachedProperties.value(qobject_cast<QGraphicsLayoutItem*>(wid))) {
    int row = static_cast<GridLayoutAttached *>(obj)->row();
    int column = static_cast<GridLayoutAttached *>(obj)->column();
    int rowSpan = static_cast<GridLayoutAttached *>(obj)->rowSpan();
    int columnSpan = static_cast<GridLayoutAttached *>(obj)->columnSpan();
    if (row == -1 || column == -1) {
        qWarning() << "Must set row and column for an item in a grid layout";
        return;
    }
    addItem(wid, row, column, rowSpan, columnSpan);
}
}

void QGraphicsGridLayoutObject::addLayoutItem(QGraphicsLayoutItem *item)
{
//use attached properties
if (GridLayoutAttached *obj = attachedProperties.value(item)) {
    int row = obj->row();
    int column = obj->column();
    int rowSpan = obj->rowSpan();
    int columnSpan = obj->columnSpan();
    Qt::Alignment alignment = obj->alignment();
    if (row == -1 || column == -1) {
        qWarning() << "Must set row and column for an item in a grid layout";
        return;
    }
    if(obj->rowSpacing() != -1)
        setRowSpacing(row, obj->rowSpacing());
    if(obj->columnSpacing() != -1)
        setColumnSpacing(column, obj->columnSpacing());
    if(obj->rowStretchFactor() != -1)
        setRowStretchFactor(row, obj->rowStretchFactor());
    if(obj->columnStretchFactor() != -1)
        setColumnStretchFactor(column, obj->columnStretchFactor());
    if(obj->rowPreferredHeight() != -1)
        setRowPreferredHeight(row, obj->rowPreferredHeight());
    if(obj->rowMaximumHeight() != -1)
        setRowMaximumHeight(row, obj->rowMaximumHeight());
    if(obj->rowMinimumHeight() != -1)
        setRowMinimumHeight(row, obj->rowMinimumHeight());
    if(obj->rowFixedHeight() != -1)
        setRowFixedHeight(row, obj->rowFixedHeight());
    if(obj->columnPreferredWidth() != -1)
        setColumnPreferredWidth(row, obj->columnPreferredWidth());
    if(obj->columnMaximumWidth() != -1)
        setColumnMaximumWidth(row, obj->columnMaximumWidth());
    if(obj->columnMinimumWidth() != -1)
        setColumnMinimumWidth(row, obj->columnMinimumWidth());
    if(obj->columnFixedWidth() != -1)
        setColumnFixedWidth(row, obj->columnFixedWidth());
    addItem(item, row, column, rowSpan, columnSpan);
    if (alignment != -1)
        setAlignment(item,alignment);
    QObject::connect(obj, SIGNAL(alignmentChanged(QGraphicsLayoutItem*,Qt::Alignment)),
                     this, SLOT(updateAlignment(QGraphicsLayoutItem*,Qt::Alignment)));
    //### need to disconnect when widget is removed?
}
}

//### is there a better way to do this?
void QGraphicsGridLayoutObject::clearChildren()
{
for (int i = 0; i < count(); ++i)
    removeAt(i);
}

qreal QGraphicsGridLayoutObject::spacing() const
{
if (verticalSpacing() == horizontalSpacing())
    return verticalSpacing();
return -1;  //###
}

qreal QGraphicsGridLayoutObject::contentsMargin() const
{
    qreal a,b,c,d;
    getContentsMargins(&a, &b, &c, &d);
    if(a==b && a==c && a==d)
        return a;
    return -1;
}

void QGraphicsGridLayoutObject::setContentsMargin(qreal m)
{
    setContentsMargins(m,m,m,m);
}


void QGraphicsGridLayoutObject::updateAlignment(QGraphicsLayoutItem *item, Qt::Alignment alignment)
{
QGraphicsGridLayout::setAlignment(item, alignment);
}

QHash<QGraphicsLayoutItem*, GridLayoutAttached*> QGraphicsGridLayoutObject::attachedProperties;
GridLayoutAttached *QGraphicsGridLayoutObject::qmlAttachedProperties(QObject *obj)
{
// ### This is not allowed - you must attach to any object
if (!qobject_cast<QGraphicsLayoutItem*>(obj))
    return 0;
GridLayoutAttached *rv = new GridLayoutAttached(obj);
attachedProperties.insert(qobject_cast<QGraphicsLayoutItem*>(obj), rv);
return rv;
}

QT_END_NAMESPACE
