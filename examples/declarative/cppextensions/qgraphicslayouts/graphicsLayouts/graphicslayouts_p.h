/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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
    Q_PROPERTY(qreal contentsMargin READ contentsMargin WRITE setContentsMargin)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsLinearLayoutObject(QObject * = 0);
    ~QGraphicsLinearLayoutObject();

    QDeclarativeListProperty<QGraphicsLayoutItem> children() { return QDeclarativeListProperty<QGraphicsLayoutItem>(this, 0, children_append, children_count, children_at, children_clear); }

    static LinearLayoutAttached *qmlAttachedProperties(QObject *);

    qreal contentsMargin() const;
    void setContentsMargin(qreal);

private Q_SLOTS:
    void updateStretch(QGraphicsLayoutItem*,int);
    void updateSpacing(QGraphicsLayoutItem*,int);
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
    Q_PROPERTY(qreal contentsMargin READ contentsMargin WRITE setContentsMargin)
    Q_PROPERTY(qreal verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)
    Q_PROPERTY(qreal horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsGridLayoutObject(QObject * = 0);
    ~QGraphicsGridLayoutObject();

    QDeclarativeListProperty<QGraphicsLayoutItem> children() { return QDeclarativeListProperty<QGraphicsLayoutItem>(this, 0, children_append, children_count, children_at, children_clear); }

    qreal spacing() const;
    qreal contentsMargin() const;
    void setContentsMargin(qreal);

    static GridLayoutAttached *qmlAttachedProperties(QObject *);

private Q_SLOTS:
    void updateAlignment(QGraphicsLayoutItem*,Qt::Alignment);

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
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
public:
    LinearLayoutAttached(QObject *parent);

    int stretchFactor() const { return _stretch; }
    void setStretchFactor(int f);
    Qt::Alignment alignment() const { return _alignment; }
    void setAlignment(Qt::Alignment a);
    int spacing() const { return _spacing; }
    void setSpacing(int s);

Q_SIGNALS:
    void stretchChanged(QGraphicsLayoutItem*,int);
    void alignmentChanged(QGraphicsLayoutItem*,Qt::Alignment);
    void spacingChanged(QGraphicsLayoutItem*,int);

private:
    int _stretch;
    Qt::Alignment _alignment;
    int _spacing;
};

class GridLayoutAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int row READ row WRITE setRow)
    Q_PROPERTY(int column READ column WRITE setColumn)
    Q_PROPERTY(int rowSpan READ rowSpan WRITE setRowSpan)
    Q_PROPERTY(int columnSpan READ columnSpan WRITE setColumnSpan)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(int rowStretchFactor READ rowStretchFactor WRITE setRowStretchFactor)
    Q_PROPERTY(int columnStretchFactor READ columnStretchFactor WRITE setColumnStretchFactor)
    Q_PROPERTY(int rowSpacing READ rowSpacing WRITE setRowSpacing)
    Q_PROPERTY(int columnSpacing READ columnSpacing WRITE setColumnSpacing)
    Q_PROPERTY(int rowPreferredHeight READ rowPreferredHeight WRITE setRowPreferredHeight)
    Q_PROPERTY(int rowMinimumHeight READ rowMinimumHeight WRITE setRowMinimumHeight)
    Q_PROPERTY(int rowMaximumHeight READ rowMaximumHeight WRITE setRowMaximumHeight)
    Q_PROPERTY(int rowFixedHeight READ rowFixedHeight WRITE setRowFixedHeight)
    Q_PROPERTY(int columnPreferredWidth READ columnPreferredWidth WRITE setColumnPreferredWidth)
    Q_PROPERTY(int columnMaximumWidth READ columnMaximumWidth WRITE setColumnMaximumWidth)
    Q_PROPERTY(int columnMinimumWidth READ columnMinimumWidth WRITE setColumnMinimumWidth)
    Q_PROPERTY(int columnFixedWidth READ columnFixedWidth WRITE setColumnFixedWidth)

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

    int rowStretchFactor() const { return _rowstretch; }
    void setRowStretchFactor(int f);
    int columnStretchFactor() const { return _colstretch; }
    void setColumnStretchFactor(int f);

    int rowSpacing() const { return _rowspacing; }
    void setRowSpacing(int s);
    int columnSpacing() const { return _colspacing; }
    void setColumnSpacing(int s);

    int rowPreferredHeight() const { return _rowprefheight; }
    void setRowPreferredHeight(int s) { _rowprefheight = s; }

    int rowMaximumHeight() const { return _rowmaxheight; }
    void setRowMaximumHeight(int s) { _rowmaxheight = s; }

    int rowMinimumHeight() const { return _rowminheight; }
    void setRowMinimumHeight(int s) { _rowminheight = s; }

    int rowFixedHeight() const { return _rowfixheight; }
    void setRowFixedHeight(int s) { _rowfixheight = s; }

    int columnPreferredWidth() const { return _colprefwidth; }
    void setColumnPreferredWidth(int s) { _colprefwidth = s; }

    int columnMaximumWidth() const { return _colmaxwidth; }
    void setColumnMaximumWidth(int s) { _colmaxwidth = s; }

    int columnMinimumWidth() const { return _colminwidth; }
    void setColumnMinimumWidth(int s) { _colminwidth = s; }

    int columnFixedWidth() const { return _colfixwidth; }
    void setColumnFixedWidth(int s) { _colfixwidth = s; }

Q_SIGNALS:
    void alignmentChanged(QGraphicsLayoutItem*,Qt::Alignment);

private:
    int _row;
    int _column;
    int _rowspan;
    int _colspan;
    Qt::Alignment _alignment;
    int _rowstretch;
    int _colstretch;
    int _rowspacing;
    int _colspacing;
    int _rowprefheight;
    int _rowmaxheight;
    int _rowminheight;
    int _rowfixheight;
    int _colprefwidth;
    int _colmaxwidth;
    int _colminwidth;
    int _colfixwidth;
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
