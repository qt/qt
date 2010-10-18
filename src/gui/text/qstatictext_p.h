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

#ifndef QSTATICTEXT_P_H
#define QSTATICTEXT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include "qstatictext.h"

#include <private/qtextureglyphcache_p.h>
#include <QtGui/qcolor.h>

QT_BEGIN_NAMESPACE

class QStaticTextUserData
{
public:
    enum Type {
        NoUserData,
        OpenGLUserData
    };

    QStaticTextUserData(Type t) : type(t) {}
    virtual ~QStaticTextUserData() {}

    Type type;
};

class Q_GUI_EXPORT QStaticTextItem
{
public:    
    QStaticTextItem() : chars(0), numChars(0), fontEngine(0), userData(0),
                        useBackendOptimizations(false), userDataNeedsUpdate(0) {}
    ~QStaticTextItem() { delete userData; }

    void setUserData(QStaticTextUserData *newUserData)
    {
        if (userData == newUserData)
            return;

        delete userData;
        userData = newUserData;
    }

    union {
        QFixedPoint *glyphPositions;             // 8 bytes per glyph
        int positionOffset;
    };
    union {
        glyph_t *glyphs;                         // 4 bytes per glyph
        int glyphOffset;
    };
    union {
        QChar *chars;                            // 2 bytes per glyph
        int charOffset;
    };
                                                 // =================
                                                 // 14 bytes per glyph

                                                 // 12 bytes for pointers
    int numGlyphs;                               // 4 bytes per item
    int numChars;                                // 4 bytes per item
    QFontEngine *fontEngine;                     // 4 bytes per item
    QFont font;                                  // 8 bytes per item
    QColor color;                                // 10 bytes per item
    QStaticTextUserData *userData;               // 8 bytes per item
    char useBackendOptimizations : 1;            // 1 byte per item
    char userDataNeedsUpdate : 1;                //
                                                 // ================
                                                 // 51 bytes per item
};

class QStaticText;
class Q_AUTOTEST_EXPORT QStaticTextPrivate
{
public:
    QStaticTextPrivate();
    QStaticTextPrivate(const QStaticTextPrivate &other);
    ~QStaticTextPrivate();

    void init();
    void paintText(const QPointF &pos, QPainter *p);

    void invalidate()
    {
        needsRelayout = true;
    }

    QAtomicInt ref;                      // 4 bytes per text

    QString text;                        // 4 bytes per text
    QFont font;                          // 8 bytes per text
    qreal textWidth;                     // 8 bytes per text
    QSizeF actualSize;                   // 16 bytes per text
    QPointF position;                    // 16 bytes per text

    QTransform matrix;                   // 80 bytes per text
    QStaticTextItem *items;              // 4 bytes per text
    int itemCount;                       // 4 bytes per text

    glyph_t *glyphPool;                  // 4 bytes per text
    QFixedPoint *positionPool;           // 4 bytes per text
    QChar *charPool;                     // 4 bytes per text

    QTextOption textOption;              // 28 bytes per text

    unsigned char needsRelayout            : 1; // 1 byte per text
    unsigned char useBackendOptimizations  : 1;
    unsigned char textFormat               : 2;
    unsigned char untransformedCoordinates : 1;
                                         // ================
                                         // 195 bytes per text

    static QStaticTextPrivate *get(const QStaticText *q);
};

QT_END_NAMESPACE

#endif // QSTATICTEXT_P_H
