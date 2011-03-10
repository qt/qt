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

#include "qsgcontext.h"
#include "renderer.h"
#include "node.h"

#include "qmlrenderer.h"
#include "default/default_rectanglenode.h"
#include "default/default_texturenode.h"
#include "default/default_glyphnode.h"
#include "distancefield_glyphnode.h"
#include "distancefieldfontatlas_p.h"

#include "qsgtexturemanager.h"

#ifdef Q_WS_MAC
#include "qsgmactexturemanager_mac_p.h"
#endif

#include <QApplication>
#include <QGLContext>

//#ifdef Q_WS_MAC
//#include "mactexturemanager.h"
//#endif

//#ifdef Q_WS_QPA
//#include "qsgeglfsthreadedtexturemanager.h"
//#endif

#include <private/qobject_p.h>

class QSGContextPrivate : public QObjectPrivate
{
public:
    QSGContextPrivate()
        : rootNode(0)
        , renderer(0)
        , gl(0)
    {
    }

    RootNode *rootNode;
    Renderer *renderer;
    QSGTextureManager *textureManager;

    QGLContext *gl;
};


/*!
    \brief The QSGContext holds the scene graph entry points for one QML engine.

    The context is not ready for use until it has a QGLContext. Once that happens,
    the scene graph population can start.
 */

QSGContext::QSGContext(QObject *parent) :
    QObject(*(new QSGContextPrivate), parent)
{
}


/*!
    Returns the renderer. The renderer instance is created through the adaptation layer.
 */
Renderer *QSGContext::renderer() const
{
    Q_D(const QSGContext);
    return d->renderer;
}


/*!
    Returns the root node. The root node instance is only created once the scene graph
    context becomes ready.
 */
RootNode *QSGContext::rootNode() const
{
    Q_D(const QSGContext);
    return d->rootNode;
}


QGLContext *QSGContext::glContext() const
{
    Q_D(const QSGContext);
    return d->gl;
}

QSGContext *QSGContext::current;

/*!
    Initializes the scene graph context with the GL context \a context. This also
    emits the ready() signal so that the QML graph can start building scene graph nodes.
 */
void QSGContext::initialize(QGLContext *context)
{
    Q_D(QSGContext);

    Q_ASSERT(!d->gl);

    d->gl = context;

    d->renderer = createRenderer();
    d->renderer->setClearColor(Qt::white);

    d->rootNode = new RootNode();
    d->renderer->setRootNode(d->rootNode);

    d->textureManager = createTextureManager(this);

    current = this;

    emit ready();
}

/*!
    Returns the texture manager for this scene graphc context. The
    texture manager is constructed through one call to createTextureManager()
    during the scene graph context's initialization
 */
QSGTextureManager *QSGContext::textureManager() const
{
    Q_D(const QSGContext);
    return d->textureManager;
}


/*!
    Returns if the scene graph context is ready or not, meaning that it has a valid
    GL context.
 */
bool QSGContext::isReady() const
{
    Q_D(const QSGContext);
    return d->gl;
}


void QSGContext::renderNextFrame()
{
    Q_D(QSGContext);

//     printf("\nFRAME:\n");

    emit aboutToRenderNextFrame();

    d->renderer->renderScene();
}

/*!
    Factory function for scene graph backends of the Rectangle element.
 */
RectangleNodeInterface *QSGContext::createRectangleNode()
{
    return new DefaultRectangleNode(DefaultRectangleNode::PreferTextureMaterial, this);
}

/*!
    Factory function for scene graph backends of the Image element.
 */
TextureNodeInterface *QSGContext::createTextureNode()
{
    return new DefaultTextureNode;
}

/*!
    Factory function for scene graph backends of the Text elements;
 */
GlyphNodeInterface *QSGContext::createGlyphNode()
{
    if (DistanceFieldFontAtlas::distanceFieldEnabled())
        return new DistanceFieldGlyphNode;
    else
        return new DefaultGlyphNode;
}

/*!
    Factory function for the scene graph renderers.

    The renderers are used for the toplevel renderer and once for every
    ShaderEffectSource used in the QML scene.
 */
Renderer *QSGContext::createRenderer()
{
    QMLRenderer *renderer = new QMLRenderer;
    if (qApp->arguments().contains("--opaque-front-to-back")) {
        printf("QSGContext: Sorting opaque nodes front to back...\n");
        renderer->setSortFrontToBackEnabled(true);
    }
    return renderer;
}

/*!
    Factory function for the texture manager to be used for this scene graph.
 */
QSGTextureManager *QSGContext::createTextureManager(QSGContext *context)
{
    QStringList args = qApp->arguments();

    QSGTextureManager *manager;

//    if (args.contains("--partial-texture-manager")) {
//        printf("QSGContext: Using partial upload texture manager\n");
//        manager = new QSGPartialUploadTextureManager;

//    } else if (args.contains("--basic-texture-manager")) {
//         printf("QSGContext: Using basic texture manager\n");
//         manager = new QSGTextureManager;

//    } else if (args.contains("--threaded-texture-manager")) {
//        printf("QSGContext: Using threaded texture manager\n");
//        manager = new QSGThreadedTextureManager;

//    } else {
#ifdef Q_WS_MAC
        manager = new QSGMacTextureManager;
#else
        manager = new QSGTextureManager;
#endif
//    }

//#if defined (Q_WS_MAC)
//    printf("QSGContext:: Using Mac Texture manager\n");
//    return new QSGMacTextureManager;
//#elif defined (Q_WS_WIN)
//    printf("QSGContext:: Using Threaded Texture Manager\n");
//    return new QSGThreadedTextureManager;
//#elif defined (Q_WS_QPA)
//    printf("QSGContext:: Using EglFS Threaded Texture Manager\n");
//    return new QSGEglFSThreadedTextureManager;
//#endif

    return manager;
}
