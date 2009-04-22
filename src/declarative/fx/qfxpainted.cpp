/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qfxpainted.h"
#include "qfxpainted_p.h"


QT_BEGIN_NAMESPACE
/*!
    \class QFxPainted
    \brief QFxPainted is an abstract base class for QFxView items that paint using QPainter.

    \ingroup coreitems

    This is a convenience class allowing easy use of QPainter within a custom item.
    The contents of the item are cached behind the scenes. Any time you change the contents
    you should call markDirty to make sure the cache is refreshed the next time painting occurs.

    \code
    class GradientRect : public QFxPainted
    {
        Q_OBJECT
    public:
        GradientRect() : QFxPainted()
        {
            connect(this, SIGNAL(widthChanged()), this, SLOT(markDirty()));
            connect(this, SIGNAL(heightChanged()), this, SLOT(markDirty()));
        }

        void paint(QPainter *painter)
        {
            painter->fillRect(0, 0, width(), height(), QBrush(QLinearGradient(0,0,width(),height())));
        }
    };
    \endcode

    \warning Dirty is only ever automatically set by QFxPainted at construction. Any other changes (including resizes) to the item need to be handled by the subclass (as in the example above).
    \warning Frequent calls to markDirty will result in sub-optimal painting performance.
*/

/*!
    Constructs the painted item.
*/

//### what options do we need to set?
QFxPainted::QFxPainted(QFxItem *parent)
  : QFxItem(*(new QFxPaintedPrivate), parent)
{
    setOptions(HasContents, true);
}

/*!
    Destroys the item.
*/
QFxPainted::~QFxPainted()
{
}

/*! \internal
*/
QFxPainted::QFxPainted(QFxPaintedPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    setOptions(HasContents, true);
}

/*!
    \fn QFxPainted::paint(QPainter *painter)

    Implement this method to paint the item.  The painting will be cached and
    paint() will only be called again if \l markDirty() has been called.

    \sa markDirty()
*/

/*!
    The contents of the item are cached behind the scenes. Any time you change the contents
    you should call markDirty to make sure the cache is refreshed the next time painting occurs.
*/
void QFxPainted::markDirty()
{
    Q_D(QFxPainted);
    if (d->dirty)
        return;
    d->dirty = true;

    // release cache memory (will be reallocated upon paintContents, if visible)
#if defined(QFX_RENDER_QPAINTER)
    d->cachedImage = QImage();
#elif defined(QFX_RENDER_OPENGL)
    d->cachedTexture.clear();
#endif

    update();
}

#if defined(QFX_RENDER_QPAINTER) 
void QFxPainted::paintContents(QPainter &p)
{
    Q_D(QFxPainted);
    if (d->dirty) {
        d->cachedImage = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
        d->cachedImage.fill(0);
        QPainter painter(&(d->cachedImage));
        paint(&painter);
        d->dirty = false;
    }
    p.drawImage(0, 0, d->cachedImage);
}
#elif defined(QFX_RENDER_OPENGL2)
void QFxPainted::paintGLContents(GLPainter &painter)
{
    Q_D(QFxPainted);
    if (d->dirty) {
        QImage img = QImage(width(), height(), QImage::Format_ARGB32);
        img.fill(0);
        QPainter p(&(img));
        paint(&p);
        d->cachedTexture.setImage(img);
        d->dirty = false;
    }

    //### mainly copied from QFxImage code
    QGLShaderProgram *shader = painter.useTextureShader();

    GLfloat vertices[8];
    GLfloat texVertices[8];
    GLTexture *tex = &d->cachedTexture;

    float widthV = d->cachedTexture.width();
    float heightV = d->cachedTexture.height();

    vertices[0] = 0; vertices[1] = heightV;
    vertices[2] = widthV; vertices[3] = heightV;
    vertices[4] = 0; vertices[5] = 0;
    vertices[6] = widthV; vertices[7] = 0;

    texVertices[0] = 0; texVertices[1] = 0;
    texVertices[2] = 1; texVertices[3] = 0;
    texVertices[4] = 0; texVertices[5] = 1;
    texVertices[6] = 1; texVertices[7] = 1;

    shader->setAttributeArray(SingleTextureShader::Vertices, vertices, 2);
    shader->setAttributeArray(SingleTextureShader::TextureCoords, texVertices, 2);

    glBindTexture(GL_TEXTURE_2D, tex->texture());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    shader->disableAttributeArray(SingleTextureShader::Vertices);
    shader->disableAttributeArray(SingleTextureShader::TextureCoords);
}

#endif

QT_END_NAMESPACE
