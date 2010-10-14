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
#include "private/qdeclarativetextlayout_p.h"

#include <qdeclarative.h>

#include <QtGui/qtextlayout.h>

QT_BEGIN_NAMESPACE

class QTextLayout;
class QTextDocumentWithImageResources;

class QDeclarativeTextPrivate : public QDeclarativeItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeText)
public:
    QDeclarativeTextPrivate();

    ~QDeclarativeTextPrivate();

    void updateSize();
    void updateLayout();

    QString text;
    QFont font;
    QColor  color;
    QDeclarativeText::TextStyle style;
    QColor  styleColor;
    QString activeLink;
    QDeclarativeText::HAlignment hAlign;
    QDeclarativeText::VAlignment vAlign;
    QDeclarativeText::TextElideMode elideMode;
    QDeclarativeText::TextFormat format;
    QDeclarativeText::WrapMode wrapMode;

    void invalidateImageCache();
    void checkImageCache();
    QPixmap imageCache;

    bool imageCacheDirty:1;
    bool updateOnComponentComplete:1;
    bool richText:1;
    bool singleline:1;
    bool cacheAllTextAsImage:1;
    bool internalWidthUpdate:1;

    QSize layedOutTextSize;
    
    void ensureDoc();
    QPixmap textDocumentImage(bool drawStyle);
    QTextDocumentWithImageResources *doc;

    QSize setupTextLayout();
    QPixmap textLayoutImage(bool drawStyle);
    void drawTextLayout(QPainter *p, const QPointF &pos, bool drawStyle);
    QDeclarativeTextLayout layout;

    static QPixmap drawOutline(const QPixmap &source, const QPixmap &styleSource);
    static QPixmap drawOutline(const QPixmap &source, const QPixmap &styleSource, int yOffset);

    static inline QDeclarativeTextPrivate *get(QDeclarativeText *t) {
        return t->d_func();
    }
};

QT_END_NAMESPACE
#endif
