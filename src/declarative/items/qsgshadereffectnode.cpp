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

#include <private/qsgshadereffectnode_p.h>

#include "qsgshadereffectmesh_p.h"
#include <private/qsgtextureprovider_p.h>
#include <private/qsgrenderer_p.h>

QT_BEGIN_NAMESPACE

class QSGCustomMaterialShader : public QSGMaterialShader
{
public:
    QSGCustomMaterialShader(const QSGShaderEffectMaterialKey &key, const QVector<QByteArray> &attributes);
    virtual void deactivate();
    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect);
    virtual char const *const *attributeNames() const;

protected:
    friend class QSGShaderEffectNode;

    virtual void initialize();
    virtual const char *vertexShader() const;
    virtual const char *fragmentShader() const;

    const QSGShaderEffectMaterialKey m_key;
    QVector<const char *> m_attributeNames;
    const QVector<QByteArray> m_attributes;

    QVector<int> m_uniformLocs;
    int m_opacityLoc;
    int m_matrixLoc;
    uint m_textureIndicesSet;
};

QSGCustomMaterialShader::QSGCustomMaterialShader(const QSGShaderEffectMaterialKey &key, const QVector<QByteArray> &attributes)
    : m_key(key)
    , m_attributes(attributes)
    , m_textureIndicesSet(false)
{
    for (int i = 0; i < attributes.count(); ++i)
        m_attributeNames.append(attributes.at(i).constData());
    m_attributeNames.append(0);
}

void QSGCustomMaterialShader::deactivate()
{
    glDisable(GL_CULL_FACE);
}

void QSGCustomMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(newEffect != 0);

    const QSGShaderEffectMaterial *material = static_cast<const QSGShaderEffectMaterial *>(newEffect);

    if (!m_textureIndicesSet) {
        for (int i = 0; i < material->m_textures.size(); ++i)
            m_program.setUniformValue(material->m_textures.at(i).first.constData(), i);
        m_textureIndicesSet = true;
    }

    if (m_uniformLocs.size() != material->m_uniformValues.size()) {
        m_uniformLocs.reserve(material->m_uniformValues.size());
        for (int i = 0; i < material->m_uniformValues.size(); ++i) {
            const QByteArray &name = material->m_uniformValues.at(i).first;
            m_uniformLocs.append(m_program.uniformLocation(name.constData()));
        }
    }

    QGLFunctions *functions = state.context()->functions();
    for (int i = material->m_textures.size() - 1; i >= 0; --i) {
        QPointer<QSGItem> source = material->m_textures.at(i).second;
        QSGTextureProvider *provider = QSGTextureProvider::from(source);
        QSGTexture *texture = provider ? provider->texture() : 0;
        if (!source || !provider || !texture) {
            qWarning("ShaderEffectItem: source or provider missing when binding textures");
            continue;
        }
        functions->glActiveTexture(GL_TEXTURE0 + i);
        provider->texture()->bind();
    }

    if (material->m_source.respectsOpacity)
        m_program.setUniformValue(m_opacityLoc, state.opacity());

    for (int i = 0; i < material->m_uniformValues.count(); ++i) {
        const QVariant &v = material->m_uniformValues.at(i).second;

        switch (v.type()) {
        case QVariant::Color:
            m_program.setUniformValue(m_uniformLocs.at(i), qt_premultiply_color(qvariant_cast<QColor>(v)));
            break;
        case QVariant::Double:
            m_program.setUniformValue(m_uniformLocs.at(i), (float) qvariant_cast<double>(v));
            break;
        case QVariant::Transform:
            m_program.setUniformValue(m_uniformLocs.at(i), qvariant_cast<QTransform>(v));
            break;
        case QVariant::Int:
            m_program.setUniformValue(m_uniformLocs.at(i), v.toInt());
            break;
        case QVariant::Bool:
            m_program.setUniformValue(m_uniformLocs.at(i), GLint(v.toBool()));
            break;
        case QVariant::Size:
        case QVariant::SizeF:
            m_program.setUniformValue(m_uniformLocs.at(i), v.toSizeF());
            break;
        case QVariant::Point:
        case QVariant::PointF:
            m_program.setUniformValue(m_uniformLocs.at(i), v.toPointF());
            break;
        case QVariant::Rect:
        case QVariant::RectF:
            {
                QRectF r = v.toRectF();
                m_program.setUniformValue(m_uniformLocs.at(i), r.x(), r.y(), r.width(), r.height());
            }
            break;
        case QVariant::Vector3D:
            m_program.setUniformValue(m_uniformLocs.at(i), qvariant_cast<QVector3D>(v));
            break;
        default:
            break;
        }
    }

    const QSGShaderEffectMaterial *oldMaterial = static_cast<const QSGShaderEffectMaterial *>(oldEffect);
    if (oldEffect == 0 || material->cullMode() != oldMaterial->cullMode()) {
        switch (material->cullMode()) {
        case QSGShaderEffectMaterial::FrontFaceCulling:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;
        case QSGShaderEffectMaterial::BackFaceCulling:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;
        default:
            glDisable(GL_CULL_FACE);
            break;
        }
    }

    if ((state.isMatrixDirty()) && material->m_source.respectsMatrix)
        m_program.setUniformValue(m_matrixLoc, state.combinedMatrix());
}

char const *const *QSGCustomMaterialShader::attributeNames() const
{
    return m_attributeNames.constData();
}

void QSGCustomMaterialShader::initialize()
{
    m_opacityLoc = m_program.uniformLocation("qt_Opacity");
    m_matrixLoc = m_program.uniformLocation("qt_ModelViewProjectionMatrix");
}

const char *QSGCustomMaterialShader::vertexShader() const
{
    return m_key.vertexCode.constData();
}

const char *QSGCustomMaterialShader::fragmentShader() const
{
    return m_key.fragmentCode.constData();
}


bool QSGShaderEffectMaterialKey::operator == (const QSGShaderEffectMaterialKey &other) const
{
    return vertexCode == other.vertexCode && fragmentCode == other.fragmentCode && className == other.className;
}

uint qHash(const QSGShaderEffectMaterialKey &key)
{
    return qHash(qMakePair(qMakePair(key.vertexCode, key.fragmentCode), key.className));
}


QHash<QSGShaderEffectMaterialKey, QSharedPointer<QSGMaterialType> > QSGShaderEffectMaterial::materialMap;

QSGShaderEffectMaterial::QSGShaderEffectMaterial()
    : m_cullMode(NoCulling)
{
    setFlag(Blending, true);
}

QSGMaterialType *QSGShaderEffectMaterial::type() const
{
    return m_type.data();
}

QSGMaterialShader *QSGShaderEffectMaterial::createShader() const
{
    return new QSGCustomMaterialShader(m_source, m_source.attributeNames);
}

int QSGShaderEffectMaterial::compare(const QSGMaterial *other) const
{
    return this - static_cast<const QSGShaderEffectMaterial *>(other);
}

void QSGShaderEffectMaterial::setCullMode(QSGShaderEffectMaterial::CullMode face)
{
    m_cullMode = face;
}

QSGShaderEffectMaterial::CullMode QSGShaderEffectMaterial::cullMode() const
{
    return m_cullMode;
}

void QSGShaderEffectMaterial::setProgramSource(const QSGShaderEffectProgram &source)
{
    m_source = source;
    m_type = materialMap.value(m_source);
    if (m_type.isNull()) {
        m_type = QSharedPointer<QSGMaterialType>(new QSGMaterialType);
        materialMap.insert(m_source, m_type);
    }
}

void QSGShaderEffectMaterial::setUniforms(const QVector<QPair<QByteArray, QVariant> > &uniformValues)
{
    m_uniformValues = uniformValues;
}

void QSGShaderEffectMaterial::setTextureProviders(const QVector<QPair<QByteArray, QPointer<QSGItem> > > &textures)
{
    m_textures = textures;
}

const QVector<QPair<QByteArray, QPointer<QSGItem> > > &QSGShaderEffectMaterial::textureProviders() const
{
    return m_textures;
}

void QSGShaderEffectMaterial::updateTextures() const
{
    for (int i = 0; i < m_textures.size(); ++i) {
        QSGItem *item = m_textures.at(i).second;
        if (item) {
            QSGTextureProvider *provider = QSGTextureProvider::from(item);
            if (provider) {
                QSGTexture *texture = provider->texture();
                if (!texture) {
                    qWarning("QSGShaderEffectMaterial: no texture from %s [%s]",
                             qPrintable(item->objectName()),
                             item->metaObject()->className());
                }
                if (QSGDynamicTexture *t = qobject_cast<QSGDynamicTexture *>(provider->texture())) {
                    t->updateTexture();
                }
            }
        }
    }
}


QSGShaderEffectNode::QSGShaderEffectNode()
{
    QSGNode::setFlag(UsePreprocess, true);
}

QSGShaderEffectNode::~QSGShaderEffectNode()
{
}

void QSGShaderEffectNode::markDirtyTexture()
{
    markDirty(DirtyMaterial);
}

void QSGShaderEffectNode::preprocess()
{
    Q_ASSERT(material());
    static_cast<QSGShaderEffectMaterial *>(material())->updateTextures();
}

QT_END_NAMESPACE
