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

#ifndef QDECLARATIVETEXT_P_H
#define QDECLARATIVETEXT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdeclarativeitem.h"
#include "private/qdeclarativeitem_p.h"

#include <qdeclarative.h>

#include <QtGui/qtextlayout.h>

QT_BEGIN_NAMESPACE

class QTextLayout;
class QTextDocumentWithImageResources;

class QDeclarativeTextPrivate : public QDeclarativeItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeText)
public:
    QDeclarativeTextPrivate()
      : color((QRgb)0), style(QDeclarativeText::Normal),
        hAlign(QDeclarativeText::AlignLeft), vAlign(QDeclarativeText::AlignTop), elideMode(QDeclarativeText::ElideNone),
        imgDirty(true), dirty(true), richText(false), singleline(false), cache(true), doc(0),
        format(QDeclarativeText::AutoText), wrapMode(QDeclarativeText::NoWrap)
    {
#if defined(QML_NO_TEXT_CACHE)
        cache = false;
#endif
        QGraphicsItemPrivate::acceptedMouseButtons = Qt::LeftButton;
        QGraphicsItemPrivate::flags = QGraphicsItemPrivate::flags & ~QGraphicsItem::ItemHasNoContents;
    }

    ~QDeclarativeTextPrivate();

    void ensureDoc();
    void updateSize();
    void updateLayout();
    void markImgDirty() {
        Q_Q(QDeclarativeText);
        imgDirty = true;
        if (q->isComponentComplete())
            q->update();
    }
    void checkImgCache();

    void drawOutline();
    void drawOutline(int yOffset);

    QPixmap wrappedTextImage(bool drawStyle);
    void drawWrappedText(QPainter *p, const QPointF &pos, bool drawStyle);
    QPixmap richTextImage(bool drawStyle);
    QSize setupTextLayout(QTextLayout *layout);

    QString text;
    QFont font;
    QColor  color;
    QDeclarativeText::TextStyle style;
    QColor  styleColor;
    QString activeLink;
    QPixmap imgCache;
    QPixmap imgStyleCache;
    QDeclarativeText::HAlignment hAlign;
    QDeclarativeText::VAlignment vAlign;
    QDeclarativeText::TextElideMode elideMode;
    bool imgDirty:1;
    bool dirty:1;
    bool richText:1;
    bool singleline:1;
    bool cache:1;
    QTextDocumentWithImageResources *doc;
    QTextLayout layout;
    QSize cachedLayoutSize;
    QDeclarativeText::TextFormat format;
    QDeclarativeText::WrapMode wrapMode;
    
    static inline QDeclarativeTextPrivate *get(QDeclarativeText *t) {
        return t->d_func();
    }
};

QT_END_NAMESPACE
#endif
