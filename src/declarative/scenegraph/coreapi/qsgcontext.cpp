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

#include "qsgtexture_p.h"

#include <QApplication>
#include <QGLContext>

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

    current = this;

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
    QSGTexture *t = new QSGPlainTexture();
    if (!image.isNull())
        t->setImage(image);
    return t;
}
