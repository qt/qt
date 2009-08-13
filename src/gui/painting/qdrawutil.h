/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDRAWUTIL_H
#define QDRAWUTIL_H

#include <QtCore/qnamespace.h>
#include <QtCore/qstring.h> // char*->QString conversion
#include <QtGui/qpixmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QPainter;
#ifndef QT3_SUPPORT
class QColorGroup;
#endif
class QPalette;
class QPoint;
class QColor;
class QBrush;
class QRect;

//
// Standard shade drawing
//

Q_GUI_EXPORT void qDrawShadeLine(QPainter *p, int x1, int y1, int x2, int y2,
                              const QPalette &pal, bool sunken = true,
                              int lineWidth = 1, int midLineWidth = 0);

Q_GUI_EXPORT void qDrawShadeLine(QPainter *p, const QPoint &p1, const QPoint &p2,
                              const QPalette &pal, bool sunken = true,
                              int lineWidth = 1, int midLineWidth = 0);

Q_GUI_EXPORT void qDrawShadeRect(QPainter *p, int x, int y, int w, int h,
                              const QPalette &pal, bool sunken = false,
                              int lineWidth = 1, int midLineWidth = 0,
                              const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawShadeRect(QPainter *p, const QRect &r,
                              const QPalette &pal, bool sunken = false,
                              int lineWidth = 1, int midLineWidth = 0,
                              const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawShadePanel(QPainter *p, int x, int y, int w, int h,
                               const QPalette &pal, bool sunken = false,
                               int lineWidth = 1, const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawShadePanel(QPainter *p, const QRect &r,
                               const QPalette &pal, bool sunken = false,
                               int lineWidth = 1, const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawWinButton(QPainter *p, int x, int y, int w, int h,
                              const QPalette &pal, bool sunken = false,
                              const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawWinButton(QPainter *p, const QRect &r,
                              const QPalette &pal, bool sunken = false,
                              const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawWinPanel(QPainter *p, int x, int y, int w, int h,
                              const QPalette &pal, bool sunken = false,
                             const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawWinPanel(QPainter *p, const QRect &r,
                              const QPalette &pal, bool sunken = false,
                             const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawPlainRect(QPainter *p, int x, int y, int w, int h, const QColor &,
                              int lineWidth = 1, const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawPlainRect(QPainter *p, const QRect &r, const QColor &,
                              int lineWidth = 1, const QBrush *fill = 0);


#ifdef QT3_SUPPORT
//
// Use QStyle::itemRect(), QStyle::drawItem() and QStyle::drawArrow() instead.
//
Q_GUI_EXPORT QT3_SUPPORT QRect qItemRect(QPainter *p, Qt::GUIStyle gs, int x, int y, int w, int h,
                          int flags, bool enabled,
                          const QPixmap *pixmap, const QString& text, int len=-1);

Q_GUI_EXPORT QT3_SUPPORT void qDrawItem(QPainter *p, Qt::GUIStyle gs, int x, int y, int w, int h,
                         int flags, const QPalette &pal, bool enabled,
                         const QPixmap *pixmap, const QString& text,
                         int len=-1, const QColor* penColor = 0);

Q_GUI_EXPORT QT3_SUPPORT void qDrawArrow(QPainter *p, Qt::ArrowType type, Qt::GUIStyle style, bool down,
                          int x, int y, int w, int h,
                          const QPalette &pal, bool enabled);
#endif

struct QMargins
{
    inline QMargins(int margin = 0)
        : top(margin),
          left(margin),
          bottom(margin),
          right(margin) {}
    inline QMargins(int topMargin, int leftMargin, int bottomMargin, int rightMargin)
         : top(topMargin),
           left(leftMargin),
           bottom(bottomMargin),
           right(rightMargin) {}
    int top;
    int left;
    int bottom;
    int right;
};

struct QTileRules
{
    inline QTileRules(Qt::TileRule horizontalRule, Qt::TileRule verticalRule = Qt::Stretch)
            : horizontal(horizontalRule), vertical(verticalRule) {}
    inline QTileRules(Qt::TileRule rule = Qt::Stretch)
            : horizontal(rule), vertical(rule) {}
    Qt::TileRule horizontal;
    Qt::TileRule vertical;
};

Q_GUI_EXPORT void qDrawBorderPixmap(QPainter *painter, 
                                    const QRect &targetRect, 
                                    const QMargins &targetMargins, 
                                    const QPixmap &pixmap,
                                    const QRect &sourceRect, 
                                    const QMargins &sourceMargins, 
                                    const QTileRules &rules = QTileRules());
inline void qDrawBorderPixmap(QPainter *painter, 
                                           const QRect &target, 
                                           const QMargins &margins, 
                                           const QPixmap &pixmap)
{
    qDrawBorderPixmap(painter, target, margins, pixmap, pixmap.rect(), margins);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDRAWUTIL_H
