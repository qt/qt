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

#include <private/qsgshadereffectitem_p.h>
#include <private/qsgshadereffectnode_p.h>

#include "qsgmaterial.h"
#include "qsgitem_p.h"

#include <private/qsgcontext_p.h>
#include <private/qsgtextureprovider_p.h>
#include "qsgcanvas.h"

#include <QtCore/qsignalmapper.h>
#include <QtOpenGL/qglframebufferobject.h>

QT_BEGIN_NAMESPACE

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
    "varying highp vec2 qt_TexCoord0;                                   \n"
    "uniform sampler2D source;                                          \n"
    "uniform lowp float qt_Opacity;                                     \n"
    "void main() {                                                      \n"
    "    gl_FragColor = texture2D(source, qt_TexCoord0) * qt_Opacity;   \n"
    "}";

static const char qt_position_attribute_name[] = "qt_Vertex";
static const char qt_texcoord_attribute_name[] = "qt_MultiTexCoord0";

const char *qtPositionAttributeName()
{
    return qt_position_attribute_name;
}

const char *qtTexCoordAttributeName()
{
    return qt_texcoord_attribute_name;
}

QSGShaderEffectItem::QSGShaderEffectItem(QSGItem *parent)
    : QSGItem(parent)
    , m_mesh(0)
    , m_cullMode(NoCulling)
    , m_blending(true)
    , m_dirtyData(true)
    , m_programDirty(true)
    , m_dirtyMesh(true)
    , m_dirtyGeometry(true)
{
    setFlag(QSGItem::ItemHasContents);
}

QSGShaderEffectItem::~QSGShaderEffectItem()
{
    reset();
}

void QSGShaderEffectItem::componentComplete()
{
    updateProperties();
    QSGItem::componentComplete();
}

void QSGShaderEffectItem::setFragmentShader(const QByteArray &code)
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

void QSGShaderEffectItem::setVertexShader(const QByteArray &code)
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

void QSGShaderEffectItem::setBlending(bool enable)
{
    if (blending() == enable)
        return;

    m_blending = enable;
    update();

    emit blendingChanged();
}

void QSGShaderEffectItem::setMesh(QSGShaderEffectMesh *mesh)
{
    if (mesh == m_mesh)
        return;
    if (m_mesh)
        disconnect(m_mesh, SIGNAL(geometryChanged()), this, 0);
    m_mesh = mesh;
    if (m_mesh)
        connect(m_mesh, SIGNAL(geometryChanged()), this, SLOT(updateGeometry()));
    m_dirtyMesh = true;
    update();
    emit meshChanged();
}

void QSGShaderEffectItem::setCullMode(CullMode face)
{
    if (face == m_cullMode)
        return;
    m_cullMode = face;
    update();
    emit cullModeChanged();
}

void QSGShaderEffectItem::changeSource(int index)
{
    Q_ASSERT(index >= 0 && index < m_sources.size());
    QVariant v = property(m_sources.at(index).name.constData());
    setSource(v, index);
}

void QSGShaderEffectItem::updateData()
{
    m_dirtyData = true;
    update();
}

void QSGShaderEffectItem::updateGeometry()
{
    m_dirtyGeometry = true;
    update();
}

void QSGShaderEffectItem::setSource(const QVariant &var, int index)
{
    Q_ASSERT(index >= 0 && index < m_sources.size());

    SourceData &source = m_sources[index];

    source.item = 0;
    if (var.isNull()) {
        return;
    } else if (!qVariantCanConvert<QObject *>(var)) {
        qWarning("Could not assign source of type '%s' to property '%s'.", var.typeName(), source.name.constData());
        return;
    }

    QObject *obj = qVariantValue<QObject *>(var);

    QSGTextureProvider *int3rface = QSGTextureProvider::from(obj);
    if (!int3rface) {
        qWarning("Could not assign property '%s', did not implement QSGTextureProvider.", source.name.constData());
    }

    source.item = qobject_cast<QSGItem *>(obj);

    // TODO: Find better solution.
    // 'source.item' needs a canvas to get a scenegraph node.
    // The easiest way to make sure it gets a canvas is to
    // make it a part of the same item tree as 'this'.
    if (source.item && source.item->parentItem() == 0) {
        source.item->setParentItem(this);
        source.item->setVisible(false);
    }
}

void QSGShaderEffectItem::disconnectPropertySignals()
{
    disconnect(this, 0, this, SLOT(updateData()));
    for (int i = 0; i < m_sources.size(); ++i) {
        SourceData &source = m_sources[i];
        disconnect(this, 0, source.mapper, 0);
        disconnect(source.mapper, 0, this, 0);
    }
}

void QSGShaderEffectItem::connectPropertySignals()
{
    QSet<QByteArray>::const_iterator it;
    for (it = m_source.uniformNames.begin(); it != m_source.uniformNames.end(); ++it) {
        int pi = metaObject()->indexOfProperty(it->constData());
        if (pi >= 0) {
            QMetaProperty mp = metaObject()->property(pi);
            if (!mp.hasNotifySignal())
                qWarning("QSGShaderEffectItem: property '%s' does not have notification method!", it->constData());
            QByteArray signalName("2");
            signalName.append(mp.notifySignal().signature());
            connect(this, signalName, this, SLOT(updateData()));
        } else {
            qWarning("QSGShaderEffectItem: '%s' does not have a matching property!", it->constData());
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
            qWarning("QSGShaderEffectItem: '%s' does not have a matching source!", source.name.constData());
        }
    }
}

void QSGShaderEffectItem::reset()
{
    disconnectPropertySignals();

    m_source.attributeNames.clear();
    m_source.uniformNames.clear();
    m_source.respectsOpacity = false;
    m_source.respectsMatrix = false;
    m_source.className = metaObject()->className();

    for (int i = 0; i < m_sources.size(); ++i) {
        const SourceData &source = m_sources.at(i);
        delete source.mapper;
        if (source.item && source.item->parentItem() == this)
            source.item->setParentItem(0);
    }
    m_sources.clear();

    m_programDirty = true;
    m_dirtyMesh = true;
}

void QSGShaderEffectItem::updateProperties()
{
    QByteArray vertexCode = m_source.vertexCode;
    QByteArray fragmentCode = m_source.fragmentCode;
    if (vertexCode.isEmpty())
        vertexCode = qt_default_vertex_code;
    if (fragmentCode.isEmpty())
        fragmentCode = qt_default_fragment_code;

    lookThroughShaderCode(vertexCode);
    lookThroughShaderCode(fragmentCode);

    if (!m_mesh && !m_source.attributeNames.contains(qt_position_attribute_name))
        qWarning("QSGShaderEffectItem: Missing reference to \'%s\'.", qt_position_attribute_name);
    if (!m_mesh && !m_source.attributeNames.contains(qt_texcoord_attribute_name))
        qWarning("QSGShaderEffectItem: Missing reference to \'%s\'.", qt_texcoord_attribute_name);
    if (!m_source.respectsMatrix)
        qWarning("QSGShaderEffectItem: Missing reference to \'qt_ModelViewProjectionMatrix\'.");
    if (!m_source.respectsOpacity)
        qWarning("QSGShaderEffectItem: Missing reference to \'qt_Opacity\'.");

    for (int i = 0; i < m_sources.size(); ++i) {
        QVariant v = property(m_sources.at(i).name);
        setSource(v, i);
    }

    connectPropertySignals();
}

void QSGShaderEffectItem::lookThroughShaderCode(const QByteArray &code)
{
    // Regexp for matching attributes and uniforms.
    // In human readable form: attribute|uniform [lowp|mediump|highp] <type> <name>
    static QRegExp re(QLatin1String("\\b(attribute|uniform)\\b\\s*\\b(?:lowp|mediump|highp)?\\b\\s*\\b(\\w+)\\b\\s*\\b(\\w+)"));
    Q_ASSERT(re.isValid());

    int pos = -1;

    QString wideCode = QString::fromLatin1(code.constData(), code.size());

    while ((pos = re.indexIn(wideCode, pos + 1)) != -1) {
        QByteArray decl = re.cap(1).toLatin1(); // uniform or attribute
        QByteArray type = re.cap(2).toLatin1(); // type
        QByteArray name = re.cap(3).toLatin1(); // variable name

        if (decl == "attribute") {
            m_source.attributeNames.append(name);
        } else {
            Q_ASSERT(decl == "uniform");

            if (name == "qt_ModelViewProjectionMatrix") {
                m_source.respectsMatrix = true;
            } else if (name == "qt_Opacity") {
                m_source.respectsOpacity = true;
            } else {
                m_source.uniformNames.insert(name);
                if (type == "sampler2D") {
                    SourceData d;
                    d.mapper = new QSignalMapper;
                    d.name = name;
                    d.item = 0;
                    m_sources.append(d);
                }
            }
        }
    }
}

void QSGShaderEffectItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_dirtyGeometry = true;
    QSGItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode *QSGShaderEffectItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGShaderEffectNode *node = static_cast<QSGShaderEffectNode *>(oldNode);

    if (!node) {
        node = new QSGShaderEffectNode;
        node->setMaterial(&m_material);
        m_programDirty = true;
        m_dirtyData = true;
        m_dirtyGeometry = true;
    }

    if (m_dirtyMesh) {
        node->setGeometry(0);
        m_dirtyMesh = false;
        m_dirtyGeometry = true;
    }

    if (m_dirtyGeometry) {
        node->setFlag(QSGNode::OwnsGeometry, false);
        QSGGeometry *geometry = node->geometry();
        QRectF rect(0, 0, width(), height());
        QSGShaderEffectMesh *mesh = m_mesh ? m_mesh : &m_defaultMesh;

        geometry = mesh->updateGeometry(geometry, m_source.attributeNames, rect);
        if (!geometry) {
            delete node;
            return 0;
        }

        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry, true);

        m_dirtyGeometry = false;
    }

    if (m_programDirty) {
        QSGShaderEffectProgram s = m_source;
        if (s.fragmentCode.isEmpty())
            s.fragmentCode = qt_default_fragment_code;
        if (s.vertexCode.isEmpty())
            s.vertexCode = qt_default_vertex_code;

        m_material.setProgramSource(s);
        node->markDirty(QSGNode::DirtyMaterial);
        m_programDirty = false;
    }

    // Update blending
    if (bool(m_material.flags() & QSGMaterial::Blending) != m_blending) {
        m_material.setFlag(QSGMaterial::Blending, m_blending);
        node->markDirty(QSGNode::DirtyMaterial);
    }

    if (int(m_material.cullMode()) != int(m_cullMode)) {
        m_material.setCullMode(QSGShaderEffectMaterial::CullMode(m_cullMode));
        node->markDirty(QSGNode::DirtyMaterial);
    }

    if (m_dirtyData) {
        QVector<QPair<QByteArray, QVariant> > values;
        QVector<QPair<QByteArray, QPointer<QSGItem> > > textures;
        const QVector<QPair<QByteArray, QPointer<QSGItem> > > &oldTextures = m_material.textureProviders();

        for (QSet<QByteArray>::const_iterator it = m_source.uniformNames.begin(); 
             it != m_source.uniformNames.end(); ++it) {
            values.append(qMakePair(*it, property(*it)));
        }
        for (int i = 0; i < oldTextures.size(); ++i) {
            QSGTextureProvider *oldSource = QSGTextureProvider::from(oldTextures.at(i).second);
            if (oldSource && oldSource->textureChangedSignal())
                disconnect(oldTextures.at(i).second, oldSource->textureChangedSignal(), node, SLOT(markDirtyTexture()));
        }
        for (int i = 0; i < m_sources.size(); ++i) {
            const SourceData &source = m_sources.at(i);
            textures.append(qMakePair(source.name, source.item));
            QSGTextureProvider *t = QSGTextureProvider::from(source.item);
            if (t && t->textureChangedSignal())
                connect(source.item, t->textureChangedSignal(), node, SLOT(markDirtyTexture()));
        }
        m_material.setUniforms(values);
        m_material.setTextureProviders(textures);
        node->markDirty(QSGNode::DirtyMaterial);
        m_dirtyData = false;
    }

    return node;
}

QT_END_NAMESPACE
