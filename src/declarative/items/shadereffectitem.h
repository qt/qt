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

#ifndef SHADEREFFECTITEM_H
#define SHADEREFFECTITEM_H

#include "qsgitem.h"

#include "shadereffectsource.h"

#include "effectnode.h"
#include "material.h"
#include "adaptationlayer.h"

#include <QtCore/qpointer.h>

class QSGContext;
class QSignalMapper;
class CustomShaderMaterialData;

// TODO: Implement async loading and loading over network.
// TODO: Implement support for multisampling.
struct ShaderEffectProgram
{
    ShaderEffectProgram() 
    : respectsOpacity(false), respectsMatrix(false) {}

    QString vertexCode;
    QString fragmentCode;

    QVector<QGL::VertexAttribute> attributes;
    QVector<QByteArray> attributeNames;
    QSet<QByteArray> uniformNames;

    uint respectsOpacity : 1;
    uint respectsMatrix : 1;
};

class ShaderEffectItem : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(QString fragmentShader READ fragmentShader WRITE setFragmentShader NOTIFY fragmentShaderChanged)
    Q_PROPERTY(QString vertexShader READ vertexShader WRITE setVertexShader NOTIFY vertexShaderChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY blendingChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QSize meshResolution READ meshResolution WRITE setMeshResolution NOTIFY meshResolutionChanged)

public:
    ShaderEffectItem(QSGItem *parent = 0);
    ~ShaderEffectItem();

    virtual void componentComplete();

    QString fragmentShader() const { return m_source.fragmentCode; }
    void setFragmentShader(const QString &code);
    
    QString vertexShader() const { return m_source.vertexCode; }
    void setVertexShader(const QString &code);

    bool blending() const { return m_blending; }
    void setBlending(bool enable);

    bool active() const { return m_active; }
    void setActive(bool enable);

    QSize meshResolution() const { return m_mesh_resolution; }
    void setMeshResolution(const QSize &size);

    void preprocess();

Q_SIGNALS:
    void fragmentShaderChanged();
    void vertexShaderChanged();
    void blendingChanged();
    void activeChanged();
    void marginsChanged();
    void meshResolutionChanged();

protected:
    virtual Node *updatePaintNode(Node *, UpdatePaintNodeData *);

private Q_SLOTS:
    void changeSource(int index);
    void markDirty();

private:
    friend class CustomShaderMaterialData;
    friend class ShaderEffectNode;

    void setSource(QVariant var, int index);
    void disconnectPropertySignals();
    void connectPropertySignals();
    void reset();
    void updateProperties();
    void lookThroughShaderCode(const QString &code);

    ShaderEffectProgram m_source;
    QSize m_mesh_resolution;

    struct SourceData
    {
        QSignalMapper *mapper;
        QPointer<ShaderEffectSource> source;
        QByteArray name;
        bool ownedByEffect;
    };
    QVector<SourceData> m_sources;

    uint m_blending : 1;
    uint m_dirtyData : 1;

    uint m_programDirty : 1;
    uint m_active : 1;
};

#endif // SHADEREFFECTITEM_H
