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

#ifndef DISTANCEFIELDTEXTMATERIAL_H
#define DISTANCEFIELDTEXTMATERIAL_H

#include <qsgmaterial.h>
#include "qsgdistancefieldglyphnode_p.h"

QT_BEGIN_NAMESPACE

class QSGDistanceFieldGlyphCache;

class QSGDistanceFieldTextMaterial: public QSGMaterial
{
public:
    QSGDistanceFieldTextMaterial();
    ~QSGDistanceFieldTextMaterial();

    virtual QSGMaterialType *type() const;
    virtual QSGMaterialShader *createShader() const;
    virtual int compare(const QSGMaterial *other) const;

    void setColor(const QColor &color) { m_color = color; }
    const QColor &color() const { return m_color; }

    void setGlyphCache(QSGDistanceFieldGlyphCache *a) { m_glyph_cache = a; }
    QSGDistanceFieldGlyphCache *glyphCache() const { return m_glyph_cache; }

    bool updateTexture();

protected:
    QSize m_size;
    QColor m_color;
    QSGDistanceFieldGlyphCache *m_glyph_cache;
};

class QSGDistanceFieldStyledTextMaterial : public QSGDistanceFieldTextMaterial
{
public:
    QSGDistanceFieldStyledTextMaterial();
    ~QSGDistanceFieldStyledTextMaterial();

    virtual QSGMaterialType *type() const = 0;
    virtual QSGMaterialShader *createShader() const = 0;
    virtual int compare(const QSGMaterial *other) const;

    void setStyleColor(const QColor &color) { m_styleColor = color; }
    const QColor &styleColor() const { return m_styleColor; }

protected:
    QColor m_styleColor;
};

class QSGDistanceFieldOutlineTextMaterial : public QSGDistanceFieldStyledTextMaterial
{
public:
    QSGDistanceFieldOutlineTextMaterial();
    ~QSGDistanceFieldOutlineTextMaterial();

    virtual QSGMaterialType *type() const;
    virtual QSGMaterialShader *createShader() const;
};

class QSGDistanceFieldShiftedStyleTextMaterial : public QSGDistanceFieldStyledTextMaterial
{
public:
    QSGDistanceFieldShiftedStyleTextMaterial();
    ~QSGDistanceFieldShiftedStyleTextMaterial();

    virtual QSGMaterialType *type() const;
    virtual QSGMaterialShader *createShader() const;

    void setShift(const QPointF &shift) { m_shift = shift; }
    const QPointF &shift() const { return m_shift; }

protected:
    QPointF m_shift;
};

class QSGSubPixelDistanceFieldTextMaterial : public QSGDistanceFieldTextMaterial
{
public:
    virtual QSGMaterialType *type() const;
    virtual QSGMaterialShader *createShader() const;
};

QT_END_NAMESPACE

#endif // DISTANCEFIELDTEXTMATERIAL_H
