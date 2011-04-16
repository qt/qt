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

#include "qsgrenderer_p.h"
#include "qsgnode.h"
#include "qsgmaterial.h"
#include "qsgnodeupdater_p.h"

#include "private/qsgadaptationlayer_p.h"

#include <QGLShaderProgram>
#include <qglframebufferobject.h>
#include <QtGui/qapplication.h>

#include <qdatetime.h>

QT_BEGIN_NAMESPACE

//#define RENDERER_DEBUG
//#define QT_GL_NO_SCISSOR_TEST

// #define QSG_RENDERER_TIMING
#ifdef QSG_RENDERER_TIMING
static QTime frameTimer;
static int preprocessTime;
static int updatePassTime;
static int frameNumber = 0;
#endif

void Bindable::clear(QSGRenderer::ClearMode mode) const
{
    GLuint bits = 0;
    if (mode & QSGRenderer::ClearColorBuffer) bits |= GL_COLOR_BUFFER_BIT;
    if (mode & QSGRenderer::ClearDepthBuffer) bits |= GL_DEPTH_BUFFER_BIT;
    if (mode & QSGRenderer::ClearStencilBuffer) bits |= GL_STENCIL_BUFFER_BIT;
    glClear(bits);
}

// Reactivate the color buffer after switching to the stencil.
void Bindable::reactivate() const
{
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

BindableFbo::BindableFbo(QGLFramebufferObject *fbo) : m_fbo(fbo)
{
}


void BindableFbo::bind() const
{
    m_fbo->bind();
}

/*!
    \class QSGRenderer
    \brief The renderer class is the abstract baseclass use for rendering the
    QML scene graph.

    The renderer is not tied to any particular surface. It expects a context to
    be current and will render into that surface according to how the device rect,
    viewport rect and projection transformation are set up.

    Rendering is a sequence of steps initiated by calling renderScene(). This will
    effectively draw the scene graph starting at the root node. The QSGNode::preprocess()
    function will be called for all the nodes in the graph, followed by an update
    pass which updates all matrices, opacity, clip states and similar in the graph.
    Because the update pass is called after preprocess, it is safe to modify the graph
    during preprocess. To run a custom update pass over the graph, install a custom
    QSGNodeUpdater using setNodeUpdater(). Once all the graphs dirty states are updated,
    the virtual render() function is called.

    The render() function is implemented by QSGRenderer subclasses to render the graph
    in the most optimal way for a given hardware.

    The renderer can make use of stencil, depth and color buffers in addition to the
    scissor rect.

    \internal
 */


QSGRenderer::QSGRenderer(QSGContext *context)
    : QObject()
    , m_clear_color(Qt::transparent)
    , m_clear_mode(ClearColorBuffer | ClearDepthBuffer)
    , m_render_opacity(1)
    , m_context(context)
    , m_root_node(0)
    , m_node_updater(0)
    , m_changed_emitted(false)
    , m_mirrored(false)
    , m_is_rendering(false)
    , m_bindable(0)
{
    initializeGLFunctions();
}


QSGRenderer::~QSGRenderer()
{
    setRootNode(0);
    delete m_node_updater;
}

/*!
    Returns the scene graph context for this renderer.

    \internal
 */

QSGContext *QSGRenderer::context()
{
    return m_context;
}




/*!
    Returns the node updater that this renderer uses to update states in the
    scene graph.

    If no updater is specified a default one is constructed.
 */

QSGNodeUpdater *QSGRenderer::nodeUpdater() const
{
    if (!m_node_updater)
        const_cast<QSGRenderer *>(this)->m_node_updater = new QSGNodeUpdater();
    return m_node_updater;
}


/*!
    Sets the node updater that this renderer uses to update states in the
    scene graph.

    This will delete and override any existing node updater
  */
void QSGRenderer::setNodeUpdater(QSGNodeUpdater *updater)
{
    if (m_node_updater)
        delete m_node_updater;
    m_node_updater = updater;
}


void QSGRenderer::setRootNode(QSGRootNode *node)
{
    if (m_root_node == node)
        return;
    if (m_root_node) {
        m_root_node->m_renderers.removeOne(this);
        nodeChanged(m_root_node, QSGNode::DirtyNodeRemoved);
    }
    m_root_node = node;
    if (m_root_node) {
        Q_ASSERT(!m_root_node->m_renderers.contains(this));
        m_root_node->m_renderers << this;
        nodeChanged(m_root_node, QSGNode::DirtyNodeAdded);
    }
}


void QSGRenderer::renderScene()
{
    class B : public Bindable
    {
    public:
        B() : m_ctx(const_cast<QGLContext *>(QGLContext::currentContext())) { }
        void bind() const { QGLFramebufferObject::bindDefault(); }
    private:
        QGLContext *m_ctx;
    } b;
    renderScene(b);
}

void QSGRenderer::renderScene(const Bindable &bindable)
{
    if (!m_root_node)
        return;

    m_is_rendering = true;
#ifdef QSG_RENDERER_TIMING
    frameTimer.start();
#endif

    m_bindable = &bindable;
    preprocess();

    bindable.bind();
#ifdef QSG_RENDERER_TIMING
    int bindTime = frameTimer.elapsed();
#endif

    render();
#ifdef QSG_RENDERER_TIMING
    int renderTime = frameTimer.elapsed();
#endif

    glDisable(GL_SCISSOR_TEST);
    m_is_rendering = false;
    m_changed_emitted = false;
    m_bindable = 0;

#ifdef QSG_RENDERER_TIMING
    printf("Frame #%d: Breakdown of frametime: preprocess=%d, updates=%d, binding=%d, render=%d, total=%d\n",
           ++frameNumber,
           preprocessTime,
           updatePassTime - preprocessTime,
           bindTime - updatePassTime,
           renderTime - bindTime,
           renderTime);
#endif
}

void QSGRenderer::setProjectMatrixToDeviceRect()
{
    setProjectMatrixToRect(m_device_rect);
}

void QSGRenderer::setProjectMatrixToRect(const QRectF &rect)
{
    QMatrix4x4 matrix;
    matrix.ortho(rect.x(),
                 rect.x() + rect.width(),
                 rect.y() + rect.height(),
                 rect.y(),
                 qreal(0.01),
                 -1);
    setProjectMatrix(matrix);
}

void QSGRenderer::setProjectMatrix(const QMatrix4x4 &matrix)
{
    m_projection_matrix = matrix;
    // Mirrored relative to the usual Qt coordinate system with origin in the top left corner.
    m_mirrored = matrix(0, 0) * matrix(1, 1) - matrix(0, 1) * matrix(1, 0) > 0;
}

void QSGRenderer::setClearColor(const QColor &color)
{
    m_clear_color = color;
}

void QSGRenderer::nodeChanged(QSGNode *node, QSGNode::DirtyFlags flags)
{
    Q_UNUSED(node);
    Q_UNUSED(flags);

    if (flags & QSGNode::DirtyNodeAdded)
        addNodesToPreprocess(node);
    if (flags & QSGNode::DirtyNodeRemoved)
        removeNodesToPreprocess(node);

    if (!m_changed_emitted && !m_is_rendering) {
        // Premature overoptimization to avoid excessive signal emissions
        m_changed_emitted = true;
        emit sceneGraphChanged();
    }
}

void QSGRenderer::materialChanged(QSGGeometryNode *, QSGMaterial *, QSGMaterial *)
{
}

void QSGRenderer::preprocess()
{
    Q_ASSERT(m_root_node);

    // We need to take a copy here, in case any of the preprocess calls deletes a node that
    // is in the preprocess list and thus, changes the m_nodes_to_preprocess behind our backs
    // For the default case, when this does not happen, the cost is neglishible.
    QSet<QSGNode *> items = m_nodes_to_preprocess;

    for (QSet<QSGNode *>::const_iterator it = items.constBegin();
         it != items.constEnd(); ++it) {
        QSGNode *n = *it;
        if (!nodeUpdater()->isNodeBlocked(n, m_root_node)) {
            n->preprocess();
        }
    }

#ifdef QSG_RENDERER_TIMING
    preprocessTime = frameTimer.elapsed();
#endif

    nodeUpdater()->setToplevelOpacity(context()->renderAlpha());
    nodeUpdater()->updateStates(m_root_node);

#ifdef QSG_RENDERER_TIMING
    updatePassTime = frameTimer.elapsed();
#endif

}

void QSGRenderer::addNodesToPreprocess(QSGNode *node)
{
    for (int i = 0; i < node->childCount(); ++i)
        addNodesToPreprocess(node->childAtIndex(i));
    if (node->flags() & QSGNode::UsePreprocess)
        m_nodes_to_preprocess.insert(node);
}

void QSGRenderer::removeNodesToPreprocess(QSGNode *node)
{
    for (int i = 0; i < node->childCount(); ++i)
        removeNodesToPreprocess(node->childAtIndex(i));
    if (node->flags() & QSGNode::UsePreprocess)
        m_nodes_to_preprocess.remove(node);
}


/*!
    Convenience function to set up the stencil buffer for clipping based on \a clip.

    If the clip is a pixel aligned rectangle, this function will use glScissor instead
    of stencil.
 */

QSGRenderer::ClipType QSGRenderer::updateStencilClip(const QSGClipNode *clip)
{
    if (!clip) {
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_SCISSOR_TEST);
        return NoClip;
    }

    bool stencilEnabled = false;
    bool scissorEnabled = false;

    glDisable(GL_SCISSOR_TEST);

    int clipDepth = 0;
    QRect clipRect;
    while (clip) {
        QMatrix4x4 matrix = m_projectionMatrix.top();
        if (clip->matrix())
            matrix *= *clip->matrix();

        const QMatrix4x4 &m = matrix;

        // TODO: Check for multisampling and pixel grid alignment.
        bool canUseScissor = clip->isRectangular()
                           && qFuzzyIsNull(m(0, 1)) && qFuzzyIsNull(m(0, 2))
                           && qFuzzyIsNull(m(1, 0)) && qFuzzyIsNull(m(1, 2));

        if (canUseScissor) {
            QRectF bbox = clip->clipRect();
            qreal invW = 1 / m(3, 3);
            qreal fx1 = (bbox.left() * m(0, 0) + m(0, 3)) * invW;
            qreal fy1 = (bbox.bottom() * m(1, 1) + m(1, 3)) * invW;
            qreal fx2 = (bbox.right() * m(0, 0) + m(0, 3)) * invW;
            qreal fy2 = (bbox.top() * m(1, 1) + m(1, 3)) * invW;

            GLint ix1 = qRound((fx1 + 1) * m_device_rect.width() * qreal(0.5));
            GLint iy1 = qRound((fy1 + 1) * m_device_rect.height() * qreal(0.5));
            GLint ix2 = qRound((fx2 + 1) * m_device_rect.width() * qreal(0.5));
            GLint iy2 = qRound((fy2 + 1) * m_device_rect.height() * qreal(0.5));

            if (!scissorEnabled) {
                clipRect = QRect(ix1, iy1, ix2 - ix1, iy2 - iy1);
                glEnable(GL_SCISSOR_TEST);
                scissorEnabled = true;
            } else {
                clipRect &= QRect(ix1, iy1, ix2 - ix1, iy2 - iy1);
            }

            clipRect = clipRect.normalized();
            glScissor(clipRect.x(), clipRect.y(), clipRect.width(), clipRect.height());
        } else {
            if (!stencilEnabled) {
                if (!m_clip_program.isLinked()) {
                    m_clip_program.addShaderFromSourceCode(QGLShader::Vertex,
                        "attribute highp vec4 vCoord;       \n"
                        "uniform highp mat4 matrix;         \n"
                        "void main() {                      \n"
                        "    gl_Position = matrix * vCoord; \n"
                        "}");
                    m_clip_program.addShaderFromSourceCode(QGLShader::Fragment,
                        "void main() {                                   \n"
                        "    gl_FragColor = vec4(0.81, 0.83, 0.12, 1.0); \n" // Trolltech green ftw!
                        "}");
                    m_clip_program.bindAttributeLocation("vCoord", 0);
                    m_clip_program.link();
                    m_clip_matrix_id = m_clip_program.uniformLocation("matrix");
                }

                glStencilMask(0xff); // write mask
                glClearStencil(0);
                glClear(GL_STENCIL_BUFFER_BIT);
                glEnable(GL_STENCIL_TEST);
                glDisable(GL_DEPTH_TEST);
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                glDepthMask(GL_FALSE);

                m_clip_program.bind();
                m_clip_program.enableAttributeArray(0);

                stencilEnabled = true;
            }

            glStencilFunc(GL_EQUAL, clipDepth, 0xff); // stencil test, ref, test mask
            glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); // stencil fail, z fail, z pass

            const QSGGeometry *geometry = clip->geometry();
            Q_ASSERT(geometry->attributeCount() > 0);
            const QSGGeometry::Attribute *a = geometry->attributes();

            glVertexAttribPointer(0, a->tupleSize, a->type, GL_FALSE, geometry->stride(), geometry->vertexData());

            m_clip_program.setUniformValue(m_clip_matrix_id, m);
            draw(clip);

            ++clipDepth;
        }

        clip = clip->clipList();
    }

    if (stencilEnabled) {
        m_clip_program.disableAttributeArray(0);
        glEnable(GL_DEPTH_TEST);
        glStencilFunc(GL_EQUAL, clipDepth, 0xff); // stencil test, ref, test mask
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // stencil fail, z fail, z pass
        glStencilMask(0); // write mask
        bindable()->reactivate();
        //glDepthMask(GL_TRUE); // must be reset correctly by caller.
    } else {
        glDisable(GL_STENCIL_TEST);
    }

    if (!scissorEnabled)
        glDisable(GL_SCISSOR_TEST);

    return stencilEnabled ? StencilClip : ScissorClip;
}


/*!
    Issues the GL draw call for \a geometryNode.

    The function assumes that attributes have been bound and set up prior
    to making this call.

    \internal
 */

void QSGRenderer::draw(const QSGBasicGeometryNode *node)
{
    const QSGGeometry *g = node->geometry();
    if (g->indexCount()) {
        glDrawElements(g->drawingMode(), g->indexCount(), g->indexType(), g->indexData());
    } else {
        glDrawArrays(g->drawingMode(), 0, g->vertexCount());
    }
}


static inline int size_of_type(GLenum type)
{
    static int sizes[] = {
        sizeof(char),
        sizeof(unsigned char),
        sizeof(short),
        sizeof(unsigned short),
        sizeof(int),
        sizeof(unsigned int),
        sizeof(float),
        2,
        3,
        4,
        sizeof(double)
    };
    return sizes[type - GL_BYTE];
}

/*!
    Convenience function to set up and bind the vertex data in \a g to the
    required attribute positions defined in \a material.

    \internal
 */

void QSGRenderer::bindGeometry(QSGMaterialShader *material, const QSGGeometry *g)
{
    char const *const *attrNames = material->attributeNames();
    int offset = 0;
    for (int j = 0; attrNames[j]; ++j) {
        if (!*attrNames[j])
            continue;
        Q_ASSERT_X(j < g->attributeCount(), "QSGRenderer::bindGeometry()", "Geometry lacks attribute required by material");
        const QSGGeometry::Attribute &a = g->attributes()[j];
        Q_ASSERT_X(j == a.position, "QSGRenderer::bindGeometry()", "Geometry does not have continuous attribute positions");
#if defined(QT_OPENGL_ES_2)
        GLboolean normalize = a.type != GL_FLOAT;
#else
        GLboolean normalize = a.type != GL_FLOAT && a.type != GL_DOUBLE;
#endif
        glVertexAttribPointer(a.position, a.tupleSize, a.type, normalize, g->stride(), (char *) g->vertexData() + offset);
        offset += a.tupleSize * size_of_type(a.type);
    }
}


QT_END_NAMESPACE
