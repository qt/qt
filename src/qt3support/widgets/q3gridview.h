/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q3GRIDVIEW_H
#define Q3GRIDVIEW_H

#include <Qt3Support/q3scrollview.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3GridViewPrivate;

class Q_COMPAT_EXPORT Q3GridView : public Q3ScrollView
{
    Q_OBJECT
    Q_PROPERTY(int numRows READ numRows WRITE setNumRows)
    Q_PROPERTY(int numCols READ numCols WRITE setNumCols)
    Q_PROPERTY(int cellWidth READ cellWidth WRITE setCellWidth)
    Q_PROPERTY(int cellHeight READ cellHeight WRITE setCellHeight)
public:

    Q3GridView(QWidget *parent=0, const char *name=0, Qt::WindowFlags f=0);
   ~Q3GridView();

    int numRows() const;
    virtual void setNumRows(int);
    int numCols() const;
    virtual void setNumCols(int);

    int cellWidth() const;
    virtual void setCellWidth(int);
    int cellHeight() const;
    virtual void setCellHeight(int);

    QRect cellRect() const;
    QRect cellGeometry(int row, int column);
    QSize gridSize() const;

    int rowAt(int y) const;
    int columnAt(int x) const;

    void repaintCell(int row, int column, bool erase=true);
    void updateCell(int row, int column);
    void ensureCellVisible(int row, int column);

protected:
    virtual void paintCell(QPainter *, int row, int col) = 0;
    virtual void paintEmptyArea(QPainter *p, int cx, int cy, int cw, int ch);

    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);

    virtual void dimensionChange(int, int);

private:
    void drawContents(QPainter*);
    void updateGrid();

    int nrows;
    int ncols;
    int cellw;
    int cellh;
    Q3GridViewPrivate* d;

    Q_DISABLE_COPY(Q3GridView)
};

inline int Q3GridView::cellWidth() const
{ return cellw; }

inline int Q3GridView::cellHeight() const
{ return cellh; }

inline int Q3GridView::rowAt(int y) const
{ return y / cellh; }

inline int Q3GridView::columnAt(int x) const
{ return x / cellw; }

inline int Q3GridView::numRows() const
{ return nrows; }

inline int Q3GridView::numCols() const
{return ncols; }

inline QRect Q3GridView::cellRect() const
{ return QRect(0, 0, cellw, cellh); }

inline QSize Q3GridView::gridSize() const
{ return QSize(ncols * cellw, nrows * cellh); }

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3GRIDVIEW_H
