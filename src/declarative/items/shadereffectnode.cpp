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

#include "shadereffectnode.h"

#include "shadereffectitem.h" // XXX todo

class CustomMaterialShader : public AbstractMaterialShader
{
public:
    CustomMaterialShader(ShaderEffectNode *n);
    virtual ~CustomMaterialShader();
    virtual void updateState(Renderer *renderer, AbstractMaterial *newEffect, AbstractMaterial *oldEffect, Renderer::Updates updates);
    virtual char const *const *attributeNames() const;

protected:
    friend class ShaderEffectNode;

    virtual void initialize();
    virtual const char *vertexShader() const;
    virtual const char *fragmentShader() const;

    ShaderEffectNode *m_node;
    QSharedPointer<AbstractMaterialType> m_type_obj;
};

CustomMaterialShader::CustomMaterialShader(ShaderEffectNode *n)
    : m_node(n)
{
    Q_ASSERT(m_node);
    m_node->m_shaders.append(this);
    m_type_obj = m_node->m_type_obj;
}

CustomMaterialShader::~CustomMaterialShader()
{
    if (m_node)
        m_node->m_shaders.remove(m_node->m_shaders.indexOf(this));
}

void CustomMaterialShader::updateState(Renderer *r, AbstractMaterial *newEffect, AbstractMaterial *oldEffect, Renderer::Updates updates)
{
    Q_ASSERT(oldEffect == 0);
    Q_ASSERT(newEffect != 0);
    Q_ASSERT(static_cast<ShaderEffectNode *>(newEffect) == m_node);
    Q_UNUSED(oldEffect);
    Q_UNUSED(newEffect);

    for (int i = m_node->m_textures.size() - 1; i >= 0; --i) {
        QPointer<QSGTextureProvider> source = m_node->m_textures.at(i).second;
        if (!source)
            continue;

        r->glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, source->texture()->textureId());
    }

    if (m_node->m_source.respectsOpacity)
        m_program.setUniformValue("qt_Opacity", (float) m_node->opacity());

    for (int i = 0; i < m_node->m_uniformValues.count(); ++i) {
        const QByteArray &name = m_node->m_uniformValues.at(i).first;
        const QVariant &v = m_node->m_uniformValues.at(i).second;

        switch (v.type()) {
        case QVariant::Color:
            m_program.setUniformValue(name.constData(), qvariant_cast<QColor>(v));
            break;
        case QVariant::Double:
            m_program.setUniformValue(name.constData(), (float) qvariant_cast<double>(v));
            break;
        case QVariant::Transform:
            m_program.setUniformValue(name.constData(), qvariant_cast<QTransform>(v));
            break;
        case QVariant::Int:
            m_program.setUniformValue(name.constData(), v.toInt());
            break;
        case QVariant::Size:
        case QVariant::SizeF:
            m_program.setUniformValue(name.constData(), v.toSizeF());
            break;
        case QVariant::Point:
        case QVariant::PointF:
            m_program.setUniformValue(name.constData(), v.toPointF());
            break;
        case QVariant::Rect:
        case QVariant::RectF:
            {
                QRectF r = v.toRectF();
                m_program.setUniformValue(name.constData(), r.x(), r.y(), r.width(), r.height());
            }
            break;
        case QVariant::Vector3D:
            m_program.setUniformValue(name.constData(), qvariant_cast<QVector3D>(v));
            break;
        default:
            break;
        }
    }

    if ((updates & Renderer::UpdateMatrices) && m_node->m_source.respectsMatrix)
        m_program.setUniformValue("qt_ModelViewProjectionMatrix", r->combinedMatrix());
}

char const *const *CustomMaterialShader::attributeNames() const
{
    Q_ASSERT(m_node);
    return m_node->m_source.attributeNames.constData();
}

void CustomMaterialShader::initialize()
{
    Q_ASSERT(m_node);
    if (m_program.isLinked()) {
        m_program.bind();
        for (int i = 0; i < m_node->m_textures.size(); ++i)
            m_program.setUniformValue(m_node->m_textures.at(i).first.constData(), i);
    }
}

const char *CustomMaterialShader::vertexShader() const
{
    return m_node->m_source.vertexCode.constData();
}

const char *CustomMaterialShader::fragmentShader() const
{
    return m_node->m_source.fragmentCode.constData();
}


ShaderEffectMaterial::ShaderEffectMaterial()
    : m_type_obj(new AbstractMaterialType)
{
}


ShaderEffectNode::ShaderEffectNode()
    : m_meshResolution(1, 1), m_opacity(1)
{
    Node::setFlag(UsePreprocess, true);

    QVector<QSGAttributeDescription> desc = QVector<QSGAttributeDescription>()
        << QSGAttributeDescription(0, 2, GL_FLOAT, 4 * sizeof(float))
        << QSGAttributeDescription(1, 2, GL_FLOAT, 4 * sizeof(float));
    updateGeometryDescription(desc, GL_UNSIGNED_SHORT);
    AbstractMaterial::setFlag(Blending, true);
    setMaterial(this);
}

ShaderEffectNode::~ShaderEffectNode()
{
    for (int i = 0; i < m_shaders.size(); ++i)
        m_shaders.at(i)->m_node = 0;
}

void ShaderEffectNode::setRect(const QRectF &rect)
{
    setBoundingRect(rect);
    m_dirty_geometry = true;
}

QRectF ShaderEffectNode::rect() const
{
    return boundingRect();
}

void ShaderEffectNode::setResolution(const QSize &res)
{
    m_dirty_geometry = true;
    m_meshResolution = res;
}

QSize ShaderEffectNode::resolution() const
{
    return m_meshResolution;
}

void ShaderEffectNode::update()
{
    if (m_dirty_geometry) {
        updateGeometry();
        m_dirty_geometry = false;
    }
}

void ShaderEffectNode::markDirtyTexture()
{
    markDirty(DirtyMaterial);
}

void ShaderEffectNode::updateGeometry()
{
    int vmesh = m_meshResolution.height();
    int hmesh = m_meshResolution.width();

    Geometry *g = geometry();
    g->setDrawingMode(QSG::TriangleStrip);
    g->setVertexCount((vmesh + 1) * (hmesh + 1));
    g->setIndexCount(vmesh * 2 * (hmesh + 2));

    struct V { float x, y, tx, ty; };

    V *vdata = (V *) g->vertexData();

    QRectF dstRect = boundingRect();
    QRectF srcRect(0, 1, 1, -1);
    for (int iy = 0; iy <= vmesh; ++iy) {
        float fy = iy / float(vmesh);
        float y = float(dstRect.top()) + fy * float(dstRect.height());
        float ty = float(srcRect.top()) + fy * float(srcRect.height());
        for (int ix = 0; ix <= hmesh; ++ix) {
            float fx = ix / float(hmesh);
            vdata->x = float(dstRect.left()) + fx * float(dstRect.width());
            vdata->y = y;
            vdata->tx = float(srcRect.left()) + fx * float(srcRect.width());
            vdata->ty = ty;
            ++vdata;
        }
    }

    quint16 *indices = (quint16 *)g->ushortIndexData();
    int i = 0;
    for (int iy = 0; iy < vmesh; ++iy) {
        *(indices++) = i + hmesh + 1;
        for (int ix = 0; ix <= hmesh; ++ix, ++i) {
            *(indices++) = i + hmesh + 1;
            *(indices++) = i;
        }
        *(indices++) = i - 1;
    }
    Q_ASSERT(indices == g->ushortIndexData() + g->indexCount());

    markDirty(Node::DirtyGeometry);
}

void ShaderEffectNode::invalidateShaders()
{
    for (int i = 0; i < m_shaders.size(); ++i) {
        m_shaders.at(i)->m_program.removeAllShaders();
        m_shaders.at(i)->m_compiled = false;
    }
}

void ShaderEffectNode::preprocess()
{
    for (int i = 0; i < m_textures.size(); ++i) {
        QSGTextureProvider *source = m_textures.at(i).second;
        if (source)
            source->updateTexture();
    }
}

AbstractMaterialType *ShaderEffectMaterial::type() const
{
    return m_type_obj.data();
}

int ShaderEffectMaterial::compare(const AbstractMaterial *other) const
{
    return this - static_cast<const ShaderEffectMaterial *>(other);
}


AbstractMaterialShader *ShaderEffectNode::createShader() const
{
    return new CustomMaterialShader(const_cast<ShaderEffectNode *>(this));
}

void ShaderEffectNode::setOpacity(qreal o)
{
    if (o == m_opacity)
        return;

    m_opacity = o;
    markDirty(DirtyMaterial);
}

void ShaderEffectNode::setProgramSource(const ShaderEffectProgram &source)
{
    m_source = source;
    invalidateShaders();
    markDirty(DirtyMaterial);
}

void ShaderEffectNode::setData(const QVector<QPair<QByteArray, QVariant> > &uniformValues,
                               const QVector<QPair<QByteArray, QPointer<QSGTextureProvider> > > &textures)
{
    m_uniformValues = uniformValues;
    for (int i = 0; i < m_textures.size(); ++i)
        disconnect(m_textures.at(i).second, SIGNAL(textureChanged()), this, SLOT(markDirtyTexture()));
    m_textures = textures;
    for (int i = 0; i < m_textures.size(); ++i)
        connect(m_textures.at(i).second, SIGNAL(textureChanged()), this, SLOT(markDirtyTexture()));
    markDirty(DirtyMaterial);
}
