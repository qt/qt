// Commit: aeb330e3999ef3d7ae8d94b9330471f2a2a13554
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSGTEXT_P_P_H
#define QSGTEXT_P_P_H

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

#include "qsgitem.h"
#include "qsgimplicitsizeitem_p_p.h"

#include <QtDeclarative/qdeclarative.h>
#include <QtGui/qtextlayout.h>

#include <private/qdeclarativetextlayout_p.h>

QT_BEGIN_NAMESPACE

class QTextLayout;
class QSGTextDocumentWithImageResources;
class QSGPlainTexture;

class Q_AUTOTEST_EXPORT QSGTextPrivate : public QSGImplicitSizeItemPrivate
{
    Q_DECLARE_PUBLIC(QSGText)
public:
    QSGTextPrivate();
    ~QSGTextPrivate();
    void init();

    void updateSize();
    void updateLayout();
    bool determineHorizontalAlignment();
    bool setHAlign(QSGText::HAlignment, bool forceAlign = false);
    void mirrorChange();
    QTextDocument *textDocument();

    QString text;
    QFont font;
    QFont sourceFont;
    QColor  color;
    QSGText::TextStyle style;
    QColor  styleColor;
    QString activeLink;
    QSGText::HAlignment hAlign;
    QSGText::VAlignment vAlign;
    QSGText::TextElideMode elideMode;
    QSGText::TextFormat format;
    QSGText::WrapMode wrapMode;
    qreal lineHeight;
    QSGText::LineHeightMode lineHeightMode;
    int lineCount;
    int maximumLineCount;
    int maximumLineCountValid;
    QPointF elidePos;

    static QString elideChar;

    void invalidateImageCache();
    void checkImageCache();
    QPixmap imageCache;
    QSGTexture *texture;

    bool imageCacheDirty:1;
    bool updateOnComponentComplete:1;
    bool richText:1;
    bool singleline:1;
    bool cacheAllTextAsImage:1;
    bool internalWidthUpdate:1;
    bool requireImplicitWidth:1;
    bool truncated:1;
    bool hAlignImplicit:1;
    bool rightToLeftText:1;

    QRect layedOutTextRect;
    QSize paintedSize;
    qreal naturalWidth;
    virtual qreal getImplicitWidth() const;
    
    void ensureDoc();
    QPixmap textDocumentImage(bool drawStyle);
    QSGTextDocumentWithImageResources *doc;

    QRect setupTextLayout();
    QPixmap textLayoutImage(bool drawStyle);
    void drawTextLayout(QPainter *p, const QPointF &pos, bool drawStyle);
    QTextLayout layout;

    static QPixmap drawOutline(const QPixmap &source, const QPixmap &styleSource);
    static QPixmap drawOutline(const QPixmap &source, const QPixmap &styleSource, int yOffset);

    static inline QSGTextPrivate *get(QSGText *t) {
        return t->d_func();
    }

    enum NodeType {
        NodeIsNull,
        NodeIsTexture,
        NodeIsText,
    };
    NodeType nodeType;
};

QT_END_NAMESPACE

#endif // QSGTEXT_P_P_H
