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

#include "qsgnode.h"
#include "qsgmaterial.h"
#include <private/qsgtextureprovider_p.h>
#include <qsgitem.h>

#include <QtCore/qsharedpointer.h>
#include <QtCore/qpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

struct QSGShaderEffectMaterialKey {
    QByteArray vertexCode;
    QByteArray fragmentCode;
    const char *className;

    bool operator == (const QSGShaderEffectMaterialKey &other) const;
};

uint qHash(const QSGShaderEffectMaterialKey &key);

// TODO: Implement support for multisampling.
struct QSGShaderEffectProgram : public QSGShaderEffectMaterialKey
{
    QSGShaderEffectProgram() : respectsOpacity(false), respectsMatrix(false) {}

    QVector<QByteArray> attributeNames;
    QSet<QByteArray> uniformNames;

    uint respectsOpacity : 1;
    uint respectsMatrix : 1;
};


class QSGCustomMaterialShader;
class QSGShaderEffectMaterial : public QSGMaterial
{
public:
    enum CullMode
    {
        NoCulling,
        BackFaceCulling,
        FrontFaceCulling
    };

    QSGShaderEffectMaterial();
    virtual QSGMaterialType *type() const;
    virtual QSGMaterialShader *createShader() const;
    virtual int compare(const QSGMaterial *other) const;

    void setCullMode(CullMode face);
    CullMode cullMode() const;

    void setProgramSource(const QSGShaderEffectProgram &);
    void setUniforms(const QVector<QPair<QByteArray, QVariant> > &uniformValues);
    void setTextureProviders(const QVector<QPair<QByteArray, QPointer<QSGItem> > > &textures);
    const QVector<QPair<QByteArray, QPointer<QSGItem> > > &textureProviders() const;
    void updateTextures() const;

protected:
    friend class QSGShaderEffectItem;
    friend class QSGCustomMaterialShader;

    // The type pointer needs to be unique. It is not safe to let the type object be part of the
    // QSGShaderEffectMaterial, since it can be deleted and a new one constructed on top of the old
    // one. The new QSGShaderEffectMaterial would then get the same type pointer as the old one, and
    // CustomMaterialShaders based on the old one would incorrectly be used together with the new
    // one. To guarantee that the type pointer is unique, the type object must live as long as
    // there are any CustomMaterialShaders of that type.
    QSharedPointer<QSGMaterialType> m_type;

    QSGShaderEffectProgram m_source;
    QVector<QPair<QByteArray, QVariant> > m_uniformValues;
    QVector<QPair<QByteArray, QPointer<QSGItem> > > m_textures;
    CullMode m_cullMode;

    static QHash<QSGShaderEffectMaterialKey, QSharedPointer<QSGMaterialType> > materialMap;
};


class QSGShaderEffectMesh;

class QSGShaderEffectNode : public QObject, public QSGGeometryNode
{
    Q_OBJECT
public:
    QSGShaderEffectNode();
    virtual ~QSGShaderEffectNode();

    virtual void preprocess();

private Q_SLOTS:
    void markDirtyTexture();

};

QT_END_NAMESPACE

QT_END_HEADER

#endif // SHADEREFFECTNODE_H
