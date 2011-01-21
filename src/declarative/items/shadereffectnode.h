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

#ifndef SHADEREFFECTNODE_H
#define SHADEREFFECTNODE_H

#include "node.h"
#include "material.h"
#include "shadereffectitem.h"

class ShaderEffectItem;
class ShaderEffectEffect : public AbstractEffect // XXX todo - ugly hack
{
public:
    // Inherited from AbstractEffect
    virtual AbstractEffectType *type() const;
    void setFlags(Flags flags) { AbstractEffect::setFlags(flags); }

private:
    AbstractEffectType m_type;
};

class CustomShaderMaterialData;
class ShaderEffectNode : public GeometryNode,
                         public ShaderEffectEffect

{
public:
    ShaderEffectNode(ShaderEffectItem *item);

    void setRect(const QRectF &rect);
    QRectF rect() const;

    void setResolution(const QSize &res);
    QSize resolution() const;

    // Inherited from Node
    virtual void preprocess();

    // Inherited from AbstractEffect
    virtual AbstractEffectProgram *createProgram() const;

    qreal opacity() const { return m_opacity; }
    void setOpacity(qreal o);

    void setProgramSource(const ShaderEffectProgram &);
    void setData(const QList<QPair<QByteArray, QVariant> > &uniformValues,
                 const QVector<ShaderEffectItem::SourceData> &sources);

    void update();

private:
    friend class CustomShaderMaterialData;

    void updateGeometry();

    bool m_programDirty : 1;
    bool m_dirty_geometry : 1;

    QSize m_meshResolution;

    qreal m_opacity;
    ShaderEffectProgram m_source;
    QList<QPair<QByteArray, QVariant> > m_uniformValues;
    QVector<ShaderEffectItem::SourceData> m_sources; // XXX todo

    QGLShaderProgram m_program;
    void updateShaderProgram();

    ShaderEffectItem *m_item;

};

#endif // SHADEREFFECTNODE_H
