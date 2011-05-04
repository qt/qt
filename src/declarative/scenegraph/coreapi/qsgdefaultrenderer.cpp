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


#define GL_GLEXT_PROTOTYPES

#include "qsgdefaultrenderer_p.h"
#include "qsgmaterial.h"

#include <QtCore/qvarlengtharray.h>
#include <QtGui/qapplication.h>
#include <QtCore/qpair.h>

//#define FORCE_NO_REORDER

QT_BEGIN_NAMESPACE

static bool nodeLessThan(QSGGeometryNode *a, QSGGeometryNode *b)
{
    // Sort by clip...
    if (a->clipList() != b->clipList())
        return a->clipList() < b->clipList();

    // Sort by material definition
    QSGMaterialType *aDef = a->material()->type();
    QSGMaterialType *bDef = b->material()->type();

    if (aDef != bDef)
        return aDef < bDef;

    // Sort by material state
    int cmp = a->material()->compare(b->material());
    if (cmp != 0)
        return cmp < 0;

    return a->matrix() < b->matrix();
}

static bool nodeLessThanWithRenderOrder(QSGGeometryNode *a, QSGGeometryNode *b)
{
    // Sort by clip...
    if (a->clipList() != b->clipList())
        return a->clipList() < b->clipList();

    // Sort by material definition
    QSGMaterialType *aDef = a->material()->type();
    QSGMaterialType *bDef = b->material()->type();

    if (!(a->material()->flags() & QSGMaterial::Blending)) {
        int aOrder = a->renderOrder();
        int bOrder = b->renderOrder();
        if (aOrder != bOrder)
            return aOrder > bOrder;
    }

    if (aDef != bDef)
        return aDef < bDef;

    // Sort by material state
    int cmp = a->material()->compare(b->material());
    if (cmp != 0)
        return cmp < 0;

    return a->matrix() < b->matrix();
}

// Minimum heap.
template <typename T, int prealloc = 256>
class Heap
{
public:
    void insert(const T &x);
    const T &top() const { return v[0]; }
    T pop();
    bool isEmpty() const { return v.isEmpty(); }
private:
    static int parent(int i) { return (i - 1) >> 1; }
    static int left(int i) { return (i << 1) | 1; }
    static int right(int i) { return (i + 1) << 1; }
    QVarLengthArray<T, prealloc> v;
};

template <typename T, int prealloc>
void Heap<T, prealloc>::insert(const T &x)
{
    int i = v.size();
    v.append(x);
    while (i != 0 && v[i] < v[parent(i)]) {
        qSwap(v[parent(i)], v[i]);
        i = parent(i);
    }
}

template <typename T, int prealloc>
T Heap<T, prealloc>::pop()
{
    T x = top();
    if (v.size() > 1)
        qSwap(v[0], v[v.size() - 1]);
    v.resize(v.size() - 1);
    int i = 0;
    while (left(i) < v.size()) {
        int low = left(i);
        if (right(i) < v.size() && v[right(i)] < v[low])
            low = right(i);
        if (!(v[low] < v[i]))
            break;
        qSwap(v[i], v[low]);
        i = low;
    }
    return x;
}


QMLRenderer::QMLRenderer(QSGContext *context)
    : QSGRenderer(context)
    , m_rebuild_lists(false)
    , m_needs_sorting(false)
    , m_sort_front_to_back(false)
    , m_currentRenderOrder(1)
{
    QStringList args = qApp->arguments();
#if defined(QML_RUNTIME_TESTING)
    m_render_opaque_nodes = !args.contains(QLatin1String("--no-opaque-nodes"));
    m_render_alpha_nodes = !args.contains(QLatin1String("--no-alpha-nodes"));
#endif
}

void QMLRenderer::nodeChanged(QSGNode *node, QSGNode::DirtyFlags flags)
{
    QSGRenderer::nodeChanged(node, flags);

    quint32 rebuildFlags = QSGNode::DirtyNodeAdded | QSGNode::DirtyNodeRemoved | QSGNode::DirtyMaterial | QSGNode::DirtyOpacity;

    if (flags & rebuildFlags)
        m_rebuild_lists = true;

    if (flags & (rebuildFlags | QSGNode::DirtyClipList))
        m_needs_sorting = true;
}

void QMLRenderer::render()
{
#if defined (QML_RUNTIME_TESTING)
    static bool dumpTree = qApp->arguments().contains(QLatin1String("--dump-tree"));
    if (dumpTree) {
        printf("\n\n");
        QSGNodeDumper::dump(rootNode());
    }
#endif

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    glFrontFace(isMirrored() ? GL_CW : GL_CCW);
    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDepthFunc(GL_GREATER);
#if defined(QT_OPENGL_ES)
    glClearDepthf(0);
#else
    glClearDepth(0);
#endif

    glDisable(GL_SCISSOR_TEST);
    glClearColor(m_clear_color.redF(), m_clear_color.greenF(), m_clear_color.blueF(), m_clear_color.alphaF());

    bindable()->clear(clearMode());

    QRect r = viewportRect();
    glViewport(r.x(), deviceRect().bottom() - r.bottom(), r.width(), r.height());
    m_projectionMatrix = projectMatrix();
    m_projectionMatrix.push();
    m_modelViewMatrix.setToIdentity();

    m_currentClip = 0;
    glDisable(GL_STENCIL_TEST);

    m_currentMaterial = 0;
    m_currentProgram = 0;
    m_currentMatrix = 0;

    if (m_rebuild_lists) {
        m_opaqueNodes.clear();
        m_transparentNodes.clear();
        m_currentRenderOrder = 1;
        buildLists(rootNode());
        m_rebuild_lists = false;
    }

    if (m_needs_sorting) {
        qSort(m_opaqueNodes.begin(), m_opaqueNodes.end(),
              m_sort_front_to_back
              ? nodeLessThanWithRenderOrder
              : nodeLessThan);
        m_needs_sorting = false;
    }

    m_renderOrderMatrix.setToIdentity();
    m_renderOrderMatrix.scale(1, 1, qreal(1) / m_currentRenderOrder);

    glDisable(GL_BLEND);
    glDepthMask(true);
#ifdef QML_RUNTIME_TESTING
    if (m_render_opaque_nodes)
#endif
    {
#if defined (QML_RUNTIME_TESTING)
        if (dumpTree)
            qDebug() << "Opaque Nodes:";
#endif
        renderNodes(m_opaqueNodes);
    }

    glEnable(GL_BLEND);
    glDepthMask(false);
#ifdef QML_RUNTIME_TESTING
    if (m_render_alpha_nodes)
#endif
    {
#if defined (QML_RUNTIME_TESTING)
        if (dumpTree)
            qDebug() << "Alpha Nodes:";
#endif
        renderNodes(m_transparentNodes);
    }

    if (m_currentProgram)
        m_currentProgram->deactivate();

    m_projectionMatrix.pop();
}

class Foo : public QPair<int, QSGGeometryNode *>
{
public:
    Foo() { }
    Foo(int i, QSGGeometryNode *n) : QPair<int, QSGGeometryNode *>(i, n) { }
    bool operator < (const Foo &other) const { return nodeLessThan(second, other.second); }
};

void QMLRenderer::setSortFrontToBackEnabled(bool sort)
{
    printf("setting sorting to... %d\n", sort);
    m_sort_front_to_back = sort;
}

bool QMLRenderer::isSortFrontToBackEnabled() const
{
    return m_sort_front_to_back;
}

void QMLRenderer::buildLists(QSGNode *node)
{
    if (node->isSubtreeBlocked())
        return;

    if (node->type() == QSGNode::GeometryNodeType) {
        QSGGeometryNode *geomNode = static_cast<QSGGeometryNode *>(node);
        qreal opacity = geomNode->inheritedOpacity();
        QSGMaterial *m = geomNode->activeMaterial();

#ifdef FORCE_NO_REORDER
        if (true) {
#else
        if ((m->flags() & QSGMaterial::Blending) || opacity < 1) {
#endif
            geomNode->setRenderOrder(m_currentRenderOrder - 1);
            m_transparentNodes.append(geomNode);
        } else {
            geomNode->setRenderOrder(m_currentRenderOrder);
            m_opaqueNodes.append(geomNode);
            m_currentRenderOrder += 2;
        }
    }

    int count = node->childCount();
    if (!count)
        return;

#ifdef FORCE_NO_REORDER
    static bool reorder = false;
#else
    static bool reorder = !qApp->arguments().contains(QLatin1String("--no-reorder"));
#endif

    if (reorder && count > 1 && (node->flags() & QSGNode::ChildrenDoNotOverloap)) {
        QVarLengthArray<int, 16> beginIndices(count);
        QVarLengthArray<int, 16> endIndices(count);
        int baseCount = m_transparentNodes.size();
        for (int i = 0; i < count; ++i) {
            beginIndices[i] = m_transparentNodes.size();
            buildLists(node->childAtIndex(i));
            endIndices[i] = m_transparentNodes.size();
        }

        Heap<Foo, 16> heap;
        m_tempNodes.clear();
        int childNodeCount = m_transparentNodes.size() - baseCount;
        while (childNodeCount) {
            for (int i = 0; i < count; ++i) {
                if (beginIndices[i] != endIndices[i])
                    heap.insert(Foo(i, m_transparentNodes.at(beginIndices[i]++)));
            }
            while (!heap.isEmpty()) {
                Foo foo = heap.pop();
                m_tempNodes.append(foo.second);
                --childNodeCount;
                int i = foo.first;
                if (beginIndices[i] != endIndices[i] && !nodeLessThan(m_transparentNodes.at(beginIndices[i]), foo.second))
                    heap.insert(Foo(i, m_transparentNodes.at(beginIndices[i]++)));
            }
        }
        Q_ASSERT(m_tempNodes.size() == m_transparentNodes.size() - baseCount);

        m_transparentNodes.resize(baseCount);
        m_transparentNodes << m_tempNodes;
    } else {
        for (int i = 0; i < count; ++i)
            buildLists(node->childAtIndex(i));
    }
}

void QMLRenderer::renderNodes(const QVector<QSGGeometryNode *> &list)
{
    const float scale = 1.0f / m_currentRenderOrder;
    int count = list.count();
    int currentRenderOrder = 0x80000000;

    //int clipChangeCount = 0;
    //int programChangeCount = 0;
    //int materialChangeCount = 0;

    for (int i = 0; i < count; ++i) {
        QSGGeometryNode *geomNode = list.at(i);

        QSGMaterialShader::RenderState::DirtyStates updates;

#if defined (QML_RUNTIME_TESTING)
        static bool dumpTree = qApp->arguments().contains(QLatin1String("--dump-tree"));
        if (dumpTree)
            qDebug() << geomNode;
#endif

        bool changeMatrix = m_currentMatrix != geomNode->matrix();

        if (changeMatrix) {
            m_currentMatrix = geomNode->matrix();
            if (m_currentMatrix)
                m_modelViewMatrix = *m_currentMatrix;
            else
                m_modelViewMatrix.setToIdentity();
            updates |= QSGMaterialShader::RenderState::DirtyMatrix;
        }

        bool changeOpacity = m_render_opacity != geomNode->inheritedOpacity();
        if (changeOpacity) {
            updates |= QSGMaterialShader::RenderState::DirtyOpacity;
            m_render_opacity = geomNode->inheritedOpacity();
        }


        Q_ASSERT(geomNode->activeMaterial());

        QSGMaterial *material = geomNode->activeMaterial();
        QSGMaterialShader *program = m_context->prepareMaterial(material);

        bool changeClip = geomNode->clipList() != m_currentClip;
        QSGRenderer::ClipType clipType = QSGRenderer::NoClip;
        if (changeClip) {
            clipType = updateStencilClip(geomNode->clipList());
            m_currentClip = geomNode->clipList();
#ifdef FORCE_NO_REORDER
            glDepthMask(false);
#else
            glDepthMask((material->flags() & QSGMaterial::Blending) == 0 && m_render_opacity == 1);
#endif
            //++clipChangeCount;
        }

        bool changeProgram = (changeClip && clipType == QSGRenderer::StencilClip) || m_currentProgram != program;
        if (changeProgram) {
            if (m_currentProgram)
                m_currentProgram->deactivate();
            m_currentProgram = program;
            m_currentProgram->activate();
            //++programChangeCount;
            updates |= (QSGMaterialShader::RenderState::DirtyMatrix | QSGMaterialShader::RenderState::DirtyOpacity);
        }

        bool changeRenderOrder = currentRenderOrder != geomNode->renderOrder();
        if (changeRenderOrder) {
            currentRenderOrder = geomNode->renderOrder();
            m_renderOrderMatrix(2, 3) = currentRenderOrder * scale;
            m_projectionMatrix.pop();
            m_projectionMatrix.push();
            m_projectionMatrix *= m_renderOrderMatrix;
            updates |= QSGMaterialShader::RenderState::DirtyMatrix;
        }

        if (changeProgram || m_currentMaterial != material) {
            program->updateState(state(updates), material, changeProgram ? 0 : m_currentMaterial);
            m_currentMaterial = material;
            //++materialChangeCount;
        }

        //glDepthRange((geomNode->renderOrder() + 0.1) * scale, (geomNode->renderOrder() + 0.9) * scale);

        const QSGGeometry *g = geomNode->geometry();
        bindGeometry(program, g);
        draw(geomNode);
    }
    //qDebug("Clip: %i, shader program: %i, material: %i times changed while drawing %s items",
    //    clipChangeCount, programChangeCount, materialChangeCount,
    //    &list == &m_transparentNodes ? "transparent" : "opaque");
}

QT_END_NAMESPACE
