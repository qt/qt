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

#include <private/qsgcontext_p.h>
#include <private/qsgrenderer_p.h>
#include "qsgnode.h"

#include <private/qsgdefaultrenderer_p.h>

#include <private/qsgdefaultrectanglenode_p.h>
#include <private/qsgdefaultimagenode_p.h>
#include <private/qsgdefaultglyphnode_p.h>
#include <private/qsgdistancefieldglyphnode_p.h>
#include <private/qsgdistancefieldglyphcache_p.h>

#include <private/qsgtexture_p.h>
#include <qsgengine.h>

#include <QApplication>
#include <QGLContext>

#include <private/qobject_p.h>
#include <qmutex.h>

DEFINE_BOOL_CONFIG_OPTION(qmlFlashMode, QML_FLASH_MODE)
DEFINE_BOOL_CONFIG_OPTION(qmlTranslucentMode, QML_TRANSLUCENT_MODE)

/*!
    Comments about this class from Gunnar:

    The QSGContext class is right now two things.. The first is the
    adaptation layer and central storage ground for all the things
    in the scene graph, like textures and materials. This part really
    belongs inside the scene graph coreapi.

    The other part is the QML adaptation classes, like how to implement
    rectangle nodes. This is not part of the scene graph core API, but
    more part of the QML adaptation of scene graph.

    If we ever move the scene graph core API into its own thing, this class
    needs to be split in two. Right now its one because we're lazy when it comes
    to defining plugin interfaces..

 */


QT_BEGIN_NAMESPACE

class QSGContextPrivate : public QObjectPrivate
{
public:
    QSGContextPrivate()
        : rootNode(0)
        , renderer(0)
        , gl(0)
        , flashMode(qmlFlashMode())
    {
        renderAlpha = qmlTranslucentMode() ? 0.5 : 1;
    }

    ~QSGContextPrivate() 
    {
    }

    QSGRootNode *rootNode;
    QSGRenderer *renderer;

    QGLContext *gl;

    QSGEngine engine;

    QHash<QSGMaterialType *, QSGMaterialShader *> materials;

    QMutex textureMutex;
    QList<QSGTexture *> texturesToClean;

    bool flashMode;
    float renderAlpha;
};


/*!
    \class QSGContext

    \brief The QSGContext holds the scene graph entry points for one QML engine.

    The context is not ready for use until it has a QGLContext. Once that happens,
    the scene graph population can start.

    \internal
 */

QSGContext::QSGContext(QObject *parent) :
    QObject(*(new QSGContextPrivate), parent)
{
    Q_D(QSGContext);
    d->engine.setContext(this);
}


QSGContext::~QSGContext()
{
    Q_D(QSGContext);
    delete d->renderer;
    delete d->rootNode;
    cleanupTextures();
    qDeleteAll(d->materials.values());
}

/*!
    Returns the scene graph engine for this context.

    The main purpose of the QSGEngine is to serve as a public API
    to the QSGContext.

 */
QSGEngine *QSGContext::engine() const
{
    return const_cast<QSGEngine *>(&d_func()->engine);
}

/*!
    Schedules the texture to be cleaned up on the rendering thread
    at a later time.

    The texture can be considered as deleted after this function has
    been called.
  */
void QSGContext::schdelueTextureForCleanup(QSGTexture *texture)
{
    Q_D(QSGContext);
    d->textureMutex.lock();
    Q_ASSERT(!d->texturesToClean.contains(texture));
    d->texturesToClean << texture;
    d->textureMutex.unlock();
}



/*!
    Deletes all textures that have been scheduled for cleanup
 */
void QSGContext::cleanupTextures()
{
    Q_D(QSGContext);
    d->textureMutex.lock();
    qDeleteAll(d->texturesToClean);
    d->texturesToClean.clear();
    d->textureMutex.unlock();
}

/*!
    Returns the renderer. The renderer instance is created through the adaptation layer.
 */
QSGRenderer *QSGContext::renderer() const
{
    Q_D(const QSGContext);
    return d->renderer;
}


/*!
    Returns the root node. The root node instance is only created once the scene graph
    context becomes ready.
 */
QSGRootNode *QSGContext::rootNode() const
{
    Q_D(const QSGContext);
    return d->rootNode;
}


QGLContext *QSGContext::glContext() const
{
    Q_D(const QSGContext);
    return d->gl;
}

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

    d->rootNode = new QSGRootNode();
    d->renderer->setRootNode(d->rootNode);

    emit ready();
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

    emit d->engine.beforeRendering();

    cleanupTextures();
    d->renderer->renderScene();

    emit d->engine.afterRendering();

}

/*!
    Factory function for scene graph backends of the Rectangle element.
 */
QSGRectangleNode *QSGContext::createRectangleNode()
{
    return new QSGDefaultRectangleNode(this);
}

/*!
    Factory function for scene graph backends of the Image element.
 */
QSGImageNode *QSGContext::createImageNode()
{
    return new QSGDefaultImageNode;
}

/*!
    Factory function for scene graph backends of the Text elements;
 */
QSGGlyphNode *QSGContext::createGlyphNode()
{
    if (QSGDistanceFieldGlyphCache::distanceFieldEnabled()) {
        QSGGlyphNode *node = new QSGDistanceFieldGlyphNode;
        if (qApp->arguments().contains(QLatin1String("--subpixel-antialiasing")))
            node->setPreferredAntialiasingMode(QSGGlyphNode::SubPixelAntialiasing);
        return node;
    } else {
        return new QSGDefaultGlyphNode;
    }
}

/*!
    Factory function for the scene graph renderers.

    The renderers are used for the toplevel renderer and once for every
    QSGShaderEffectSource used in the QML scene.
 */
QSGRenderer *QSGContext::createRenderer()
{
    QMLRenderer *renderer = new QMLRenderer(this);
    if (qApp->arguments().contains(QLatin1String("--opaque-front-to-back"))) {
        printf("QSGContext: Sorting opaque nodes front to back...\n");
        renderer->setSortFrontToBackEnabled(true);
    }
    return renderer;
}



/*!
    Return true if the image provider supports direct decoding of images,
    straight into textures without going through a QImage first.

    If the implementation returns true from this function, the decodeImageToTexture() function
    will be called to read data from a QIODevice, rather than QML decoding
    the image using QImageReader and passing the result to setImage().

    \warning This function will be called from outside the GUI and rendering threads
    and must not make use of OpenGL.
 */

bool QSGContext::canDecodeImageToTexture() const
{
    return true;
}



/*!
    Decode the data in \a dev directly to a texture provider of \a requestSize size.
    The size of the decoded data should be written to \a impsize.

    If the implementation fails to decode the image data, it should return 0. The
    image data will then be decoded normally.

    \warning This function will be called from outside the GUI and renderer threads
    and must not make use of GL calls.
 */

QSGTexture *QSGContext::decodeImageToTexture(QIODevice *dev,
                                             QSize *size,
                                             const QSize &requestSize)
{
    Q_UNUSED(dev);
    Q_UNUSED(size);
    Q_UNUSED(requestSize);
    return 0;
}


/*!
    Factory function for texture objects.

    If \a image is a valid image, the QSGTexture::setImage function
    will be called with \a image as argument.
 */
QSGTexture *QSGContext::createTexture(const QImage &image) const
{
    QSGPlainTexture *t = new QSGPlainTexture();
    if (!image.isNull())
        t->setImage(image);
    return t;
}


/*!
    Returns a material shader for the given material.
 */
QSGMaterialShader *QSGContext::prepareMaterial(QSGMaterial *material)
{
    Q_D(QSGContext);
    QSGMaterialType *type = material->type();
    QSGMaterialShader *shader = d->materials.value(type);
    if (shader)
        return shader;

    shader = material->createShader();
    d->materials[type] = shader;
    return shader;
}

/*!
    Sets weither the scene graph should render with flashing update rectangles or not
  */
void QSGContext::setFlashModeEnabled(bool enabled)
{
    d_func()->flashMode = enabled;
}


/*!
    Returns true if the scene graph should be rendered with flashing update rectangles
 */
bool QSGContext::isFlashModeEnabled() const
{
    return d_func()->flashMode;
}


/*!
    Sets the toplevel opacity for rendering. This value will be multiplied into all
    drawing calls where possible.

    The default value is 1. Any other value will cause artifacts and is primarily
    useful for debugging.
 */
void QSGContext::setRenderAlpha(qreal renderAlpha)
{
    d_func()->renderAlpha = renderAlpha;
}


/*!
    Returns the toplevel opacity used for rendering.

    The default value is 1.

    \sa setRenderAlpha()
 */
qreal QSGContext::renderAlpha() const
{
    return d_func()->renderAlpha;
}


QT_END_NAMESPACE
