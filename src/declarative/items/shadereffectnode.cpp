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

#include "utilities.h"

#include "shadereffectitem.h" // XXX todo

class CustomShaderMaterialData : public AbstractEffectProgram
{
public:
    CustomShaderMaterialData(ShaderEffectNode *n) : node(n) { Q_ASSERT(node); }
    virtual void activate();
    virtual void deactivate();
    virtual void updateRendererState(Renderer *renderer, Renderer::Updates updates);
    virtual void updateEffectState(Renderer *renderer, AbstractEffect *newEffect, AbstractEffect *oldEffect);
    virtual const QGL::VertexAttribute *requiredFields() const;

    ShaderEffectNode *node;
};

void CustomShaderMaterialData::activate()
{
    node->updateShaderProgram();
    node->m_program.bind();
    for (int i = 0; i < node->m_source.attributeNames.size(); ++i)
        node->m_program.enableAttributeArray(node->m_source.attributes.at(i));
}

void CustomShaderMaterialData::deactivate()
{
    for (int i = 0; i < node->m_source.attributeNames.size(); ++i)
        node->m_program.disableAttributeArray(node->m_source.attributes.at(i));
}

void CustomShaderMaterialData::updateRendererState(Renderer *renderer, Renderer::Updates updates)
{
    if ((updates & Renderer::UpdateMatrices) && node->m_source.respectsMatrix)
        node->m_program.setUniformValue("qt_ModelViewProjectionMatrix", renderer->combinedMatrix());
}

void CustomShaderMaterialData::updateEffectState(Renderer *r, AbstractEffect *newEffect, AbstractEffect *oldEffect)
{
    Q_ASSERT(oldEffect == 0);
    Q_ASSERT(newEffect != 0);
    Q_UNUSED(oldEffect);
    Q_UNUSED(newEffect);

    for (int i = node->m_sources.size() - 1; i >= 0; --i) {
        const ShaderEffectItem::SourceData &source = node->m_sources.at(i);
        if (!source.source)
            continue;

        r->glActiveTexture(GL_TEXTURE0 + i);
        source.source->bind();
    }

    if (node->m_source.respectsOpacity)
        node->m_program.setUniformValue("qt_Opacity", (float) node->opacity());

    for (int ii = 0; ii < node->m_uniformValues.count(); ++ii) {
        const QByteArray &name = node->m_uniformValues.at(ii).first;
        const QVariant &v = node->m_uniformValues.at(ii).second;

        switch (v.type()) {
        case QVariant::Color:
            node->m_program.setUniformValue(name.constData(), qvariant_cast<QColor>(v));
            break;
        case QVariant::Double:
            node->m_program.setUniformValue(name.constData(), (float) qvariant_cast<double>(v));
            break;
        case QVariant::Transform:
            node->m_program.setUniformValue(name.constData(), qvariant_cast<QTransform>(v));
            break;
        case QVariant::Int:
            node->m_program.setUniformValue(name.constData(), v.toInt());
            break;
        case QVariant::Size:
        case QVariant::SizeF:
            node->m_program.setUniformValue(name.constData(), v.toSizeF());
            break;
        case QVariant::Point:
        case QVariant::PointF:
            node->m_program.setUniformValue(name.constData(), v.toPointF());
            break;
        case QVariant::Rect:
        case QVariant::RectF:
            {
                QRectF r = v.toRectF();
                node->m_program.setUniformValue(name.constData(), r.x(), r.y(), r.width(), r.height());
            }
            break;
        case QVariant::Vector3D:
            node->m_program.setUniformValue(name.constData(), qvariant_cast<QVector3D>(v));
            break;
        default:
            break;
        }
    }
}

const QGL::VertexAttribute *CustomShaderMaterialData::requiredFields() const
{
    node->updateShaderProgram();
    return node->m_source.attributes.constData();
}

ShaderEffectNode::ShaderEffectNode(ShaderEffectItem *item)
: m_programDirty(false), m_meshResolution(1, 1), m_opacity(1), m_item(item)
{
    setFlag(UsePreprocess);
    setGeometry(Utilities::createTexturedRectGeometry(QRectF(0, 0, 1, 1), QSize(1, 1), QRectF(0, 1, 1, -1)));
    AbstractEffect::setFlags(AbstractEffect::Blending);
    setMaterial(this);
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

void ShaderEffectNode::updateGeometry()
{
    int vmesh = m_meshResolution.height();
    int hmesh = m_meshResolution.width();

    Geometry *g = geometry();
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
        *(indices++) = i;
        for (int ix = 0; ix <= hmesh; ++ix) {
            *(indices++) = i++;
            *(indices++) = i + hmesh;
        }
        *(indices++) = i + hmesh;
    }
    Q_ASSERT(indices == g->ushortIndexData() + g->indexCount());

    markDirty(Node::DirtyGeometry);
}

void ShaderEffectNode::preprocess()
{
    // XXX todo
    Q_ASSERT(m_item);
    m_item->preprocess();
}

AbstractEffectType *ShaderEffectEffect::type() const
{
    return const_cast<AbstractEffectType *>(&m_type);
}

AbstractEffectProgram *ShaderEffectNode::createProgram() const
{
    return new CustomShaderMaterialData(const_cast<ShaderEffectNode *>(this));
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
    m_programDirty = true;
    m_source = source;
    markDirty(DirtyMaterial);
}

void ShaderEffectNode::updateShaderProgram()
{
    if (!m_programDirty)
        return;

    m_program.removeAllShaders();
    m_program.addShaderFromSourceCode(QGLShader::Vertex, m_source.vertexCode);
    m_program.addShaderFromSourceCode(QGLShader::Fragment, m_source.fragmentCode);

    for (int i = 0; i < m_source.attributeNames.size(); ++i)
        m_program.bindAttributeLocation(m_source.attributeNames.at(i), m_source.attributes.at(i));

    if (!m_program.link()) {
        qWarning("ShaderEffectItem: Shader compilation failed:");
        qWarning() << m_program.log();
    }

    if (!m_source.attributes.contains(QGL::Position))
        qWarning("ShaderEffectItem: Missing reference to \'qt_Vertex\'.");
    if (!m_source.attributes.contains(QGL::TextureCoord0))
        qWarning("ShaderEffectItem: Missing reference to \'qt_MultiTexCoord0\'.");
    if (!m_source.respectsMatrix)
        qWarning("ShaderEffectItem: Missing reference to \'qt_ModelViewProjectionMatrix\'.");

    if (m_program.isLinked()) {
        m_program.bind();
        for (int i = 0; i < m_sources.size(); ++i)
            m_program.setUniformValue(m_sources.at(i).name.constData(), i);
    }

    m_programDirty = false;
}

void ShaderEffectNode::setData(const QList<QPair<QByteArray, QVariant> > &uniformValues,
                               const QVector<ShaderEffectItem::SourceData> &sources)
{
    m_uniformValues = uniformValues;
    m_sources = sources;
    markDirty(DirtyMaterial);
}
