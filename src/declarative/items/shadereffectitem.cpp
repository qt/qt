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

#include "shadereffectitem.h"
#include "shadereffectnode.h"

#include "utilities.h"
#include "material.h"
#include "qsgitem_p.h"

#include "qsgcontext.h"

#include <QtCore/qsignalmapper.h>
#include <QtOpenGL/qglframebufferobject.h>

static const char qt_default_vertex_code[] =
    "uniform highp mat4 qt_ModelViewProjectionMatrix;               \n"
    "attribute highp vec4 qt_Vertex;                                \n"
    "attribute highp vec2 qt_MultiTexCoord0;                        \n"
    "varying highp vec2 qt_TexCoord0;                               \n"
    "void main() {                                                  \n"
    "    qt_TexCoord0 = qt_MultiTexCoord0;                          \n"
    "    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;    \n"
    "}";

static const char qt_default_fragment_code[] =
    "varying highp vec2 qt_TexCoord0;                       \n"
    "uniform lowp sampler2D source;                         \n"
    "void main() {                                          \n"
    "    gl_FragColor = texture2D(source, qt_TexCoord0);    \n"
    "}";

ShaderEffectItem::ShaderEffectItem(QSGItem *parent)
    : QSGItem(parent)
    , m_mesh_resolution(1, 1)
    , m_blending(true)
    , m_dirtyData(true)
    , m_programDirty(true)
    , m_active(true)
{
    setFlag(QSGItem::ItemHasContents);
}

ShaderEffectItem::~ShaderEffectItem()
{
    reset();
}

void ShaderEffectItem::componentComplete()
{
    updateProperties();
    QSGItem::componentComplete();
}

void ShaderEffectItem::setFragmentShader(const QString &code)
{
    if (m_source.fragmentCode.constData() == code.constData())
        return;
    m_source.fragmentCode = code;
    if (isComponentComplete()) {
        reset();
        updateProperties();
    }
    emit fragmentShaderChanged();
}

void ShaderEffectItem::setVertexShader(const QString &code)
{
    if (m_source.vertexCode.constData() == code.constData())
        return;
    m_source.vertexCode = code;
    if (isComponentComplete()) {
        reset();
        updateProperties();
    }

    emit vertexShaderChanged();
}

void ShaderEffectItem::setBlending(bool enable)
{
    if (blending() == enable)
        return;

    m_blending = enable;
    update();

    emit blendingChanged();
}

void ShaderEffectItem::setActive(bool enable)
{
    Q_UNUSED(enable);
    if (m_active == enable)
        return;

    if (m_active) {
        for (int i = 0; i < m_sources.size(); ++i) {
            ShaderEffectSource *source = m_sources.at(i).source;
            if (!source)
                continue;
            disconnect(source, SIGNAL(repaintRequired()), this, SLOT(markDirty()));
            source->derefFromEffectItem();
        }
    }

    m_active = enable;

    if (m_active) {
        for (int i = 0; i < m_sources.size(); ++i) {
            ShaderEffectSource *source = m_sources.at(i).source;
            if (!source)
                continue;
            source->refFromEffectItem();
            connect(source, SIGNAL(repaintRequired()), this, SLOT(markDirty()));
        }
    }

    update();
    emit activeChanged();
}

void ShaderEffectItem::setMeshResolution(const QSize &size)
{
    if (size == m_mesh_resolution)
        return;

    m_mesh_resolution = size;
    update();
}

void ShaderEffectItem::preprocess()
{
    for (int i = 0; i < m_sources.size(); ++i) {
        ShaderEffectSource *source = m_sources.at(i).source;
        if (source)
            source->update();
    }
}

void ShaderEffectItem::changeSource(int index)
{
    Q_ASSERT(index >= 0 && index < m_sources.size());
    QVariant v = property(m_sources.at(index).name.constData());
    setSource(v, index);
}

void ShaderEffectItem::markDirty()
{
    m_dirtyData = true;
    update();
}

void ShaderEffectItem::setSource(QVariant var, int index)
{
    Q_ASSERT(index >= 0 && index < m_sources.size());

    SourceData &source = m_sources[index];

    if (m_active && source.source) {
        disconnect(source.source, SIGNAL(repaintRequired()), this, SLOT(markDirty()));
        source.source->derefFromEffectItem();
    }

    enum SourceType { Url, Item, Source, Other };
    SourceType sourceType = Other;
    QObject *obj = 0;

    if (!var.isValid()) {
        sourceType = Source; // Causes source to be set to null.
    } else if (var.type() == QVariant::Url || var.type() == QVariant::String) {
        sourceType = Url;
    } else if ((QMetaType::Type)var.type() == QMetaType::QObjectStar) {
        obj = qVariantValue<QObject *>(var);
        if (qobject_cast<QSGItem *>(obj))
            sourceType = Item;
        else if (!obj || qobject_cast<ShaderEffectSource *>(obj)) // Interpret null as ShaderEffectSource.
            sourceType = Source;
    }

    switch (sourceType) {
    case Url:
        {
            QUrl url = var.type() == QVariant::Url ? var.toUrl() : QUrl(var.toString());
            if (source.ownedByEffect && !url.isEmpty() && source.source->sourceImage() == url)
                break;
            if (source.ownedByEffect)
                delete source.source;
            source.source = new ShaderEffectSource;
            source.source->setSceneGraphContext(QSGContext::current);
            source.ownedByEffect = true;
            source.source->setSourceImage(url);
        }
        break;
    case Item:
        if (source.ownedByEffect && source.source->sourceItem() == obj)
            break;
        if (source.ownedByEffect)
            delete source.source;
        source.source = new ShaderEffectSource;
        source.source->setSceneGraphContext(QSGContext::current);
        source.ownedByEffect = true;
        source.source->setSourceItem(static_cast<QSGItem *>(obj));
        break;
    case Source:
        if (obj == source.source)
            break;
        if (source.ownedByEffect)
            delete source.source;
        source.source = static_cast<ShaderEffectSource *>(obj);
        source.ownedByEffect = false;
        break;
    default:
        qWarning("Could not assign source of type '%s' to property '%s'.", var.typeName(), source.name.constData());
        break;
    }

    if (m_active && source.source) {
        source.source->refFromEffectItem();
        connect(source.source, SIGNAL(repaintRequired()), this, SLOT(markDirty()));
    }
}

void ShaderEffectItem::disconnectPropertySignals()
{
    disconnect(this, 0, this, SLOT(markDirty()));
    for (int i = 0; i < m_sources.size(); ++i) {
        SourceData &source = m_sources[i];
        disconnect(this, 0, source.mapper, 0);
        disconnect(source.mapper, 0, this, 0);
    }
}

void ShaderEffectItem::connectPropertySignals()
{
    QSet<QByteArray>::const_iterator it;
    for (it = m_source.uniformNames.begin(); it != m_source.uniformNames.end(); ++it) {
        int pi = metaObject()->indexOfProperty(it->constData());
        if (pi >= 0) {
            QMetaProperty mp = metaObject()->property(pi);
            if (!mp.hasNotifySignal())
                qWarning("ShaderEffectItem: property '%s' does not have notification method!", it->constData());
            QByteArray signalName("2");
            signalName.append(mp.notifySignal().signature());
            connect(this, signalName, this, SLOT(markDirty()));
        } else {
            qWarning("ShaderEffectItem: '%s' does not have a matching property!", it->constData());
        }
    }
    for (int i = 0; i < m_sources.size(); ++i) {
        SourceData &source = m_sources[i];
        int pi = metaObject()->indexOfProperty(source.name.constData());
        if (pi >= 0) {
            QMetaProperty mp = metaObject()->property(pi);
            QByteArray signalName("2");
            signalName.append(mp.notifySignal().signature());
            connect(this, signalName, source.mapper, SLOT(map()));
            source.mapper->setMapping(this, i);
            connect(source.mapper, SIGNAL(mapped(int)), this, SLOT(changeSource(int)));
        } else {
            qWarning("ShaderEffectItem: '%s' does not have a matching source!", source.name.constData());
        }
    }
}

void ShaderEffectItem::reset()
{
    disconnectPropertySignals();

    m_source.attributeNames.clear();
    m_source.attributes.clear();
    m_source.uniformNames.clear();
    m_source.respectsOpacity = false;
    m_source.respectsMatrix = false;

    for (int i = 0; i < m_sources.size(); ++i) {
        const SourceData &source = m_sources.at(i);
        if (m_active && source.source)
            source.source->derefFromEffectItem();
        delete source.mapper;
        if (source.ownedByEffect)
            delete source.source;
    }
    m_sources.clear();

    m_programDirty = true;
}

void ShaderEffectItem::updateProperties()
{
    QString vertexCode = m_source.vertexCode;
    QString fragmentCode = m_source.fragmentCode;
    if (vertexCode.isEmpty())
        vertexCode = QString::fromLatin1(qt_default_vertex_code);
    if (fragmentCode.isEmpty())
        fragmentCode = QString::fromLatin1(qt_default_fragment_code);

    lookThroughShaderCode(vertexCode);
    lookThroughShaderCode(fragmentCode);

    for (int i = 0; i < m_sources.size(); ++i) {
        QVariant v = property(m_sources.at(i).name);
        setSource(v, i); // Property exists.
    }

    // Append an 'end of array' marker so that m_source.attributes.constData() can be returned in requiredFields().
    m_source.attributes.append(QSG::VertexAttribute(-1));
    connectPropertySignals();
}

void ShaderEffectItem::lookThroughShaderCode(const QString &code)
{
    // Regexp for matching attributes and uniforms.
    // In human readable form: attribute|uniform [lowp|mediump|highp] <type> <name>
    static QRegExp re(QLatin1String("\\b(attribute|uniform)\\b\\s*\\b(?:lowp|mediump|highp)?\\b\\s*\\b(\\w+)\\b\\s*\\b(\\w+)"));
    Q_ASSERT(re.isValid());

    int pos = -1;

    while ((pos = re.indexIn(code, pos + 1)) != -1) {
        QString decl = re.cap(1); // uniform or attribute
        QString type = re.cap(2); // type
        QString name = re.cap(3); // variable name

        if (decl == QLatin1String("attribute")) {
            if (name == QLatin1String("qt_Vertex")) {
                m_source.attributeNames.append(name.toLatin1());
                m_source.attributes.append(QSG::Position);
            } else if (name == QLatin1String("qt_MultiTexCoord0")) {
                m_source.attributeNames.append(name.toLatin1());
                m_source.attributes.append(QSG::TextureCoord0);
            } else {
                // TODO: Support user defined attributes.
                qWarning("ShaderEffectItem: Attribute \'%s\' not recognized.", qPrintable(name));
            }
        } else {
            Q_ASSERT(decl == QLatin1String("uniform"));

            if (name == QLatin1String("qt_ModelViewProjectionMatrix")) {
                m_source.respectsMatrix = true;
            } else if (name == QLatin1String("qt_Opacity")) {
                m_source.respectsOpacity = true;
            } else {
                m_source.uniformNames.insert(name.toLatin1());
                if (type == QLatin1String("sampler2D")) {
                    SourceData d;
                    d.mapper = new QSignalMapper;
                    d.source = 0;
                    d.name = name.toLatin1();
                    d.ownedByEffect = false;
                    m_sources.append(d);
                }
            }
        }
    }
}

Node *ShaderEffectItem::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    if (!m_active) {
        delete oldNode;
        return 0;
    }

    ShaderEffectNode *node = static_cast<ShaderEffectNode *>(oldNode);
    if (!node) {
        node = new ShaderEffectNode(this);
        m_programDirty = true;
        m_dirtyData = true;
    }

    if (m_programDirty) {
        ShaderEffectProgram s = m_source;
        if (s.fragmentCode.isEmpty())
            s.fragmentCode = qt_default_fragment_code;
        if (s.vertexCode.isEmpty())
            s.vertexCode = qt_default_vertex_code;

        node->setProgramSource(s);
        m_programDirty = false;
    }

    // Update blending
    if (((node->AbstractEffect::flags() & AbstractEffect::Blending) != 0) != m_blending) {
        node->ShaderEffectEffect::setFlags(m_blending ? AbstractEffect::Blending : AbstractEffect::Flag(0));
        node->markDirty(Node::DirtyMaterial);
    }

    if (node->resolution() != m_mesh_resolution)
        node->setResolution(m_mesh_resolution);

    node->setRect(QRectF(0, 0, width(), height()));
    node->setOpacity(data->opacity);

    if (m_dirtyData) {
        QList<QPair<QByteArray, QVariant> > values;
        for (QSet<QByteArray>::const_iterator it = m_source.uniformNames.begin(); 
             it != m_source.uniformNames.end(); ++it) {
            values.append(qMakePair(*it, property(*it)));
        }

        node->setData(values, m_sources);
        m_dirtyData = false;
    }

    node->update();

    return node;
}

