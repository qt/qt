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

#include "trivialrenderer.h"
#include "material.h"

#include <qsgattributevalue.h>

QT_BEGIN_NAMESPACE

TrivialRenderer::TrivialRenderer()
{
}

void TrivialRenderer::render()
{
    const QGLContext *ctx = QGLContext::currentContext();
    const_cast<QGLContext *>(ctx)->makeCurrent();

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDepthFunc(GL_GEQUAL);
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
    m_modelViewMatrix.push();

    m_currentClip = 0;
    glDisable(GL_STENCIL_TEST);

    renderNode(rootNode());

    m_modelViewMatrix.pop();
    m_projectionMatrix.pop();
}

void TrivialRenderer::renderNode(Node *node)
{
    if (!node->isSubtreeEnabled())
        return;

    // Render if it's a geometry node
    if (node->type() == Node::GeometryNodeType) {
        GeometryNode *geomNode = static_cast<GeometryNode *>(node);

        if (geomNode->matrix())
            m_modelViewMatrix = *geomNode->matrix();
        else
            m_modelViewMatrix.setToIdentity();

        Q_ASSERT(geomNode->material());

        if (geomNode->clipList() != m_currentClip) {
            updateStencilClip(geomNode->clipList());
            m_currentClip = geomNode->clipList();
        }

        AbstractEffectProgram *program = prepareMaterial(geomNode->material());
        program->activate();

        if (geomNode->material()->flags() & AbstractEffect::Blending) {
            glEnable(GL_BLEND);
            glDepthMask(false);
        } else {
            glDisable(GL_BLEND);
            glDepthMask(true);
        }

        program->updateEffectState(this, geomNode->material(), 0);
        program->updateRendererState(this, Renderer::UpdateMatrices);

        geomNode->geometry()->draw(program->requiredFields(), geomNode->indexRange());

        program->deactivate();
    }

    // Render children
    int count = node->childCount();
    for (int i = 0; i < count; ++i) {
        renderNode(node->childAtIndex(i));
    }
}

QT_END_NAMESPACE
