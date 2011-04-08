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

#ifndef DISTANCEFIELD_GLYPHNODE_H
#define DISTANCEFIELD_GLYPHNODE_H

#include <private/qsgadaptationlayer_p.h>
#include "qsgtexture.h"
#include <qsgtext_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGDistanceFieldGlyphCache;
class QSGDistanceFieldTextMaterial;
class QSGDistanceFieldGlyphNode: public QSGGlyphNode
{
public:
    QSGDistanceFieldGlyphNode();
    ~QSGDistanceFieldGlyphNode();

    virtual QPointF baseLine() const { return m_baseLine; }
    virtual void setGlyphs(const QPointF &position, const QGlyphs &glyphs);
    virtual void setColor(const QColor &color);

    virtual void setPreferredAntialiasingMode(AntialiasingMode mode);

    void setStyle(QSGText::TextStyle style);
    void setStyleColor(const QColor &color);

private:
    void updateGeometry();
    void updateFont();
    void updateMaterial();

    QColor m_color;
    QPointF m_baseLine;
    QSGDistanceFieldTextMaterial *m_material;
    QPointF m_position;
    QGlyphs m_glyphs;
    QSGDistanceFieldGlyphCache *m_glyph_cache;
    QSGGeometry m_geometry;
    QSGText::TextStyle m_style;
    QColor m_styleColor;
    AntialiasingMode m_antialiasingMode;
};

QT_END_HEADER

QT_END_NAMESPACE

#endif // DISTANCEFIELD_GLYPHNODE_H
