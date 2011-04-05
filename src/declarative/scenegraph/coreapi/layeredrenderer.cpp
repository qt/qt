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

#include "layeredrenderer.h"
#include "node.h"
#include <qsgattributevalue.h>

#include <qalgorithms.h>

QT_BEGIN_NAMESPACE

#ifdef Q_WS_WIN
#include <private/qglextensions_p.h>
#include <private/qgl_p.h>
#endif

// #define RENDERER_DEBUG
// #define VERTEX_MERGING

static inline bool nodeLessThan(GeometryNode *a, GeometryNode *b)
{
    bool aBlend = a->material()->flags() & AbstractEffect::Blending;
    bool bBlend = b->material()->flags() & AbstractEffect::Blending;

    // Cannot sort by render opaque/transparent until we have proper layering in the depth buffers...
    // Opaque before transparent...
    if (aBlend && !bBlend)
        return false;
    else if (!aBlend && bBlend)
        return true;

    // Then sort by increasing render order
    int aOrder = a->renderOrder();
    int bOrder = b->renderOrder();
    if (aOrder != bOrder){
        if (aBlend)
            return aOrder < bOrder;
        else
            return aOrder > bOrder;
    }

    // Sort by clip...
    if (a->clipList() != b->clipList())
        return a->clipList() < b->clipList();

    // Sort by material definition
    AbstractEffectType *aDef = a->material()->type();
    AbstractEffectType *bDef = b->material()->type();
    if (aDef != bDef)
        return aDef < bDef;

    // Sort by material state
    int cmp = a->material()->compare(b->material());
    if (cmp != 0)
        return cmp < 0;

    return a->matrix() < b->matrix();
}


static void sort(QLinkedList<GeometryNode *> &list)
{
    // Insertion sort, since list is probably almost sorted already.
    QLinkedList<GeometryNode *>::iterator current = list.begin();
    QLinkedList<GeometryNode *>::iterator previous = current;
    ++current;
    while (current != list.end()) {
        if (nodeLessThan(*current, *previous)) {
            // Current item is not in order, take it out of the list.
            GeometryNode *n = *current;
            current = list.erase(current);
            // Find the correct position.
            QLinkedList<GeometryNode *>::iterator c = previous;
            while (c != list.begin()) {
                QLinkedList<GeometryNode *>::iterator p = c;
                --p;
                if (!nodeLessThan(n, *p))
                    break;
                c = p;
            }
            // Insert into the correct position.
            list.insert(c, n);
        } else {
            previous = current;
            ++current;
        }
    }
}


class RenderListBuilder : public NodeVisitor
{
public:
    RenderListBuilder(LayeredRenderer *renderer)
        : m_renderer(renderer)
    {
    }

    void enterGeometryNode(GeometryNode *node)
    {
        QLinkedList<GeometryNode *>::iterator it = m_renderer->m_nodes.begin();
        QLinkedList<GeometryNode *>::iterator end = m_renderer->m_nodes.end();

        while (it != end) {
            if (!nodeLessThan(node, *it))
                ++it;
            else
                break;
        }

        m_renderer->m_nodes.insert(it, node);

        int order = node->renderOrder();
        if (order < m_renderer->m_lowest_render_order)
            m_renderer->m_lowest_render_order = order;

        if (order > m_renderer->m_highest_render_order)
            m_renderer->m_highest_render_order = order;
    }

    LayeredRenderer *m_renderer;
};



LayeredRenderer::LayeredRenderer()
    : m_nodes_require_sorting(false)
    , m_lowest_render_order(INT_MAX)
    , m_highest_render_order(INT_MIN)
{
}


void LayeredRenderer::render()
{
    if (m_nodes.isEmpty()) {
        m_lowest_render_order = INT_MAX;
        m_highest_render_order = INT_MIN;

        RenderListBuilder builder(this);
        builder.visitNode(rootNode());
    } else if (m_nodes_require_sorting) {
        m_lowest_render_order = INT_MAX;
        m_highest_render_order = INT_MIN;

        sort(m_nodes);

        QLinkedList<GeometryNode *>::iterator it;
        for (it = m_nodes.begin(); it != m_nodes.end(); ++it) {
            int order = (*it)->renderOrder();
            if (order < m_lowest_render_order)
                m_lowest_render_order = order;
            if (order > m_highest_render_order)
                m_highest_render_order = order;
        }

        m_nodes_require_sorting = false;
    }

#ifdef RENDERER_DEBUG
    QLinkedList<GeometryNode *>::iterator it = m_nodes.begin();
    QLinkedList<GeometryNode *>::iterator end = m_nodes.end();
    qDebug() << "LayeredRenderer::render()";
    while (it != end) {
        qDebug() << "-" << *it;
        ++it;
    }
#endif

    renderNodes(m_nodes);
}

void LayeredRenderer::nodeChanged(Node *node, Node::DirtyFlags flags)
{
    if (flags & Node::DirtyNodeAdded) {
        Q_ASSERT(!m_nodes.contains(static_cast<GeometryNode *>(node)));
        RenderListBuilder builder(this);
        builder.visitNode(node);
        // Some added nodes might be dirty and need to be sorted again after they are updated.
        m_nodes_require_sorting = true;
        Q_ASSERT((node->type() == Node::GeometryNodeType) == (m_nodes.contains(static_cast<GeometryNode *>(node))));
    } else if (flags & Node::DirtyNodeRemoved) {
        Q_ASSERT((node->type() == Node::GeometryNodeType) == (m_nodes.contains(static_cast<GeometryNode *>(node))));
        class NodeRemover : public NodeVisitor {
        public:
            NodeRemover(LayeredRenderer *renderer) : m_renderer(renderer) { }
            void enterGeometryNode(GeometryNode *node) {
                m_renderer->m_nodes.removeOne(node);
            }
            LayeredRenderer *m_renderer;
        };
        NodeRemover remover(this);
        remover.visitNode(node);
        Q_ASSERT(!m_nodes.contains(static_cast<GeometryNode *>(node)));
    } else if (flags & Node::DirtyRenderOrder) {
        m_nodes_require_sorting = true;
    }

    Renderer::nodeChanged(node, flags);
}

void LayeredRenderer::renderNodes(const QLinkedList<GeometryNode *> &nodes)
{
#ifdef RENDERER_DEBUG
    qDebug() << "Renderer::renderNodes, count:" << nodes.size();
#endif

    if (nodes.isEmpty())
        return;

    const QGLContext *ctx = QGLContext::currentContext();
    const_cast<QGLContext *>(ctx)->makeCurrent();
    m_projectionMatrix.setDirty(true);
    m_modelViewMatrix.setDirty(true);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDepthFunc(GL_GREATER);
#if defined(QT_OPENGL_ES)
    glClearDepthf(0);
#else
    glClearDepth(0);
#endif

    glClearColor(m_clear_color.redF(), m_clear_color.greenF(), m_clear_color.blueF(), m_clear_color.alphaF());
    bindable()->clear();

    QRect r = deviceRect();
    glViewport(0, 0, r.width(), r.height());
    m_projectionMatrix.push();
    m_projectionMatrix = projectMatrix();
    m_projectionMatrix.push();
    m_modelViewMatrix.push();

    //painter.update();

    const ClipNode *currentClip = 0;
    glDisable(GL_STENCIL_TEST);

    QLinkedList<GeometryNode *>::const_iterator it = nodes.constBegin();
    QLinkedList<GeometryNode *>::const_iterator end = nodes.constEnd();

    float zLayerSize = m_highest_render_order - m_lowest_render_order + 1;
    float zLayerScale = 1 / zLayerSize;

    QMatrix4x4 renderOrderMatrix;
    renderOrderMatrix.scale(1, 1, zLayerScale);

    AbstractEffectProgram *currentProgram = 0;
    AbstractEffect *currentMaterial = 0;
    const QMatrix4x4 *currentMatrix = 0;
    bool isBlending = false;
    int currentRenderOrder = m_lowest_render_order - 1;

    while (it != end) {

        GeometryNode *node = *it;
        ++it;

#ifdef RENDERER_DEBUG
        qDebug() << " --> drawing node: " << node;
#endif

        if (!node->isEnabled()) {
#ifdef RENDERER_DEBUG
            qDebug() << " ---> disabled...";
#endif
            continue;
        }

        Q_ASSERT(node->material());

        bool swapClip = node->clipList() != currentClip;

        if (swapClip) {
#ifdef RENDERER_DEBUG
            qDebug() << " ---> updating clip...";
#endif
            updateStencilClip(node->clipList());
            currentClip = node->clipList();
        }

        AbstractEffect *material = node->material();
        AbstractEffectProgram *program = prepareMaterial(material);

        // Updating the clip causes a program change, reset the program if needed.
        bool swapProgram = (program != currentProgram) || swapClip;
        bool swapMaterial = (material != currentMaterial) || swapProgram;

        if (swapProgram) {
#ifdef RENDERER_DEBUG
            qDebug() << " ---> switching program...";
#endif
            if (currentProgram)
                currentProgram->deactivate();
            currentProgram = program;
            currentProgram->activate();
        }

        if (swapMaterial) {
#ifdef RENDERER_DEBUG
            qDebug() << " ---> switching material...";
#endif
            currentProgram->updateEffectState(this, material, swapProgram ? 0 : currentMaterial);
            currentMaterial = material;
        }

//        if (node->vertexBuffer().isUploaded()) {
//#ifdef RENDERER_DEBUG
//            qDebug() << " ---> node uses uploaded vertex data";
//#endif
//            node->vertexBuffer().bind();
//        }

        bool materialUsesBlending = node->material()->flags() & AbstractEffect::Blending;

        if (materialUsesBlending && !isBlending) {
            isBlending = true;
            glEnable(GL_BLEND);
            glDepthMask(false);
        } else if (!materialUsesBlending && isBlending) {
            isBlending = false;
            glDisable(GL_BLEND);
            glDepthMask(true);
        } else if (swapClip) {
            // Reset depth mask after updating the clip.
            glDepthMask((material->flags() & AbstractEffect::Blending) == 0);
        }

        const QMatrix4x4 *matrix = node->matrix();

        bool swapMatrix = matrix != currentMatrix;
        if (swapMatrix || swapProgram) {
            if (matrix)
                m_modelViewMatrix = *matrix;
            else
                m_modelViewMatrix.setToIdentity();
            currentMatrix = matrix;
        }

        if (node->renderOrder() != currentRenderOrder) {
#ifdef RENDERER_DEBUG
            qDebug() << " ---> switching render order to" << node->renderOrder() << "scaling:" << zLayerScale;
#endif
            float lowRange = (node->renderOrder() - m_lowest_render_order) * zLayerScale;
            renderOrderMatrix(2, 3) = lowRange;
            m_projectionMatrix.pop();
            m_projectionMatrix.push();
            m_projectionMatrix *= renderOrderMatrix;
            swapMatrix = true;
            currentRenderOrder = node->renderOrder();
        }

        if (swapMatrix || swapProgram)
            currentProgram->updateRendererState(this, Renderer::UpdateMatrices);

        Geometry *geometry = node->geometry();

#ifdef VERTEX_MERGING
        if (geometry->drawingMode() == QSG::TriangleStrip) {
            // Peak ahead to see if the upcoming nodes in the list require the exact same
            // setup. If they do, we can merge them into one big VBO and upload / draw only
            // once. "it" is incremented just after the node is fechted up above, so it already
            // points to the next node...

            QLinkedList<GeometryNode *>::const_iterator nit = it;

            while (nit != end) {
                GeometryNode *next = *nit;
                if (next->matrix() != currentMatrix
                    || next->material() != currentMaterial
                    || next->clipList() != currentClip
                    || next->renderOrder() != currentRenderOrder
                    || next->geometry()->drawingMode() != geometry->drawingMode())
                    break;
                ++nit;
            }

            if (it != nit) {
                //painter.update();
                mergeAndDraw(it - 1, nit);
                it = nit;
                continue;
            }
        }
#endif

//        if (vertexBuffer.isUploaded()) {
//#ifdef RENDERER_DEBUG
//            qDebug() << " ---> node uses uploaded vertex data";
//#endif
//            vertexBuffer.bind();
//        }

#ifdef RENDERER_DEBUG
        qDebug() << " ---> drawing, vertexCount:" << geometry->vertexCount() << "indexCount:" << geometry->indexCount();
#endif

        geometry->draw(program->requiredFields(), node->indexRange());

        //if (node->vertexBuffer().isUploaded())
        //    node->vertexBuffer().release();
    }

    if (currentProgram) {
#ifdef RENDERER_DEBUG
        qDebug() << "-> disabling current material properties";
#endif
        currentProgram->deactivate();
    }

    m_modelViewMatrix.pop();
    m_projectionMatrix.pop();
    m_projectionMatrix.pop();

}

void LayeredRenderer::mergeAndDraw(NodeList::const_iterator first, NodeList::const_iterator last)
{
    m_indices.resize(0);
    m_float_vertices.resize(0);

    AbstractEffectProgram *program = prepareMaterial((*first)->material());
    const QSG::VertexAttribute *attr = program->requiredFields();

    int attributeCount = 0;
    while (attributeCount < 16 && attr[attributeCount] != QSG::VertexAttribute(-1))
        ++attributeCount;

    QSGAttributeValue values[16];
    int tupleSizes[16];

    int indexOffset = 0;
    for (NodeList::const_iterator i = first; i != last; ++i) {
        GeometryNode *n = *i;

        const Geometry *g = n->geometry();

        for (int ai = 0; ai<attributeCount; ++ai) {
            values[ai] = g->attributeValue((QSG::VertexAttribute) attr[ai]);
            tupleSizes[ai] = values[ai].tupleSize();
            Q_ASSERT(values[ai].type() == GL_FLOAT);
        }

        // Interleave the vertex data into one big chunk...
        int vertexCount = g->vertexCount();
        for (int vi = 0; vi < vertexCount; ++vi) {
            for (int ai = 0; ai < attributeCount; ++ai) {
                float *tuple = (float *)((uchar *)values[ai].data() + values[ai].stride() * vi);
                for (int dim = 0; dim < values[ai].tupleSize(); ++dim)
                    m_float_vertices.append(tuple[dim]);
            }
        }

        // Append the tri-strip indices
        if (g->indexType() == GL_UNSIGNED_SHORT) {
            const ushort *indices = g->constUshortIndexData();
            m_indices << (indexOffset + indices[0]);
            for (int ii = 0; ii < g->indexCount(); ++ii)
                m_indices << (indexOffset + indices[ii]);
            m_indices << (indexOffset + indices[g->indexCount() - 1]);
        } else {
            const uint *indices = g->constUintIndexData();
            m_indices << (indexOffset + indices[0]);
            for (int ii = 0; ii < g->indexCount(); ++ii)
                m_indices << (indexOffset + indices[ii]);
            m_indices << (indexOffset + indices[g->indexCount() - 1]);
        }
        indexOffset += vertexCount;
    }

    int vertexSize = 0; // in floats, that is...
    for (int i=0; i<attributeCount; ++i)
        vertexSize += tupleSizes[i];

#ifdef Q_WS_WIN
    // To get access to the qglextensions_p.h functions...
    QGLContext *ctx = const_cast<QGLContext *>(QGLContext::currentContext());
#endif

    int offset = 0;
    for (int ai=0; ai<attributeCount; ++ai) {
        glVertexAttribPointer(attr[ai], values[ai].tupleSize(), GL_FLOAT, false, vertexSize * sizeof(float), m_float_vertices.constData() + offset);
        offset += values[ai].tupleSize();
    }

    glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_SHORT, m_indices.constData());

}

QT_END_NAMESPACE
