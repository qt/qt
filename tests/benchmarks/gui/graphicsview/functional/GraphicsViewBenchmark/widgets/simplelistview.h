/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef SIMPLELISTVIEW_H
#define SIMPLELISTVIEW_H

#include "scrollbar.h"
#include "abstractscrollarea.h"

class SimpleListViewPrivate;

class SimpleListView : public AbstractScrollArea
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SimpleListView)

public:

    SimpleListView(QGraphicsWidget *parent = 0);
    virtual ~SimpleListView();

public:

    void addItem(QGraphicsWidget *item);
    void insertItem(int index, QGraphicsWidget *item);
    QGraphicsWidget* takeItem(int row);
    QGraphicsWidget* itemAt(int row);
    int itemCount();
    void updateListContents();

    void setTwoColumns(const bool twoColumns);
    bool twoColumns();

public slots:

    void themeChange();
#if (QT_VERSION >= 0x040600)
    bool listItemCaching() const;
    void setListItemCaching(bool enabled);
#endif

protected:

    virtual void scrollContentsBy(qreal dx, qreal dy);
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    QSizeF sizeHint(Qt::SizeHint which, 
                    const QSizeF & constraint) const;

private:

    SimpleListViewPrivate *d_ptr;
};

#endif
