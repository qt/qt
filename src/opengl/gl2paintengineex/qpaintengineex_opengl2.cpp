/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    When the active program changes, we need to update it's uniforms.
    We could track state for each program and only update stale uniforms
        - Could lead to lots of overhead if there's a lot of programs
    We could update all the uniforms when the program changes
        - Could end up updating lots of uniforms which don't need updating

    Updating uniforms should be cheap, so the overhead of updating up-to-date
    uniforms should be minimal. It's also less complex.

    Things which _may_ cause a different program to be used:
        - Change in brush/pen style
        - Change in painter opacity
        - Change in composition mode

    Whenever we set a mode on the shader manager - it needs to tell us if it had
    to switch to a different program.

    The shader manager should only switch when we tell it to. E.g. if we set a new
    brush style and then switch to transparent painter, we only want it to compile
    and use the correct program when we really need it.
*/

#include "qpaintengineex_opengl2_p.h"

#include <string.h> //for memcpy
#include <qmath.h>

#include <private/qgl_p.h>
#include <private/qmath_p.h>
#include <private/qpaintengineex_p.h>
#include <QPaintEngine>
#include <private/qpainter_p.h>
#include <private/qfontengine_p.h>
#include <private/qtextureglyphcache_p.h>

#include "qglgradientcache_p.h"
#include "qglengineshadermanager_p.h"
#include "qgl2pexvertexarray_p.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

static const GLuint QT_BRUSH_TEXTURE_UNIT       = 0;
static const GLuint QT_IMAGE_TEXTURE_UNIT       = 0; //Can be the same as brush texture unit
static const GLuint QT_MASK_TEXTURE_UNIT        = 1;
static const GLuint QT_BACKGROUND_TEXTURE_UNIT  = 2;

class QGLTextureGlyphCache : public QObject, public QTextureGlyphCache
{
    Q_OBJECT
public:
    QGLTextureGlyphCache(QGLContext *context, QFontEngineGlyphCache::Type type, const QTransform &matrix);
    ~QGLTextureGlyphCache();

    virtual void createTextureData(int width, int height);
    virtual void resizeTextureData(int width, int height);
    virtual void fillTexture(const Coord &c, glyph_t glyph);

    inline GLuint texture() const { return m_texture; }

    inline int width() const { return m_width; }
    inline int height() const { return m_height; }

    inline void setPaintEnginePrivate(QGL2PaintEngineExPrivate *p) { pex = p; }


public Q_SLOTS:
    void contextDestroyed(const QGLContext *context) {
        if (context == ctx) {
            QList<const QGLContext *> shares = qgl_share_reg()->shares(ctx);
            if (shares.isEmpty()) {
                glDeleteFramebuffers(1, &m_fbo);
                if (m_width || m_height)
                    glDeleteTextures(1, &m_texture);
                ctx = 0;
            } else {
                // since the context holding the texture is shared, and
                // about to be destroyed, we have to transfer ownership
                // of the texture to one of the share contexts
                ctx = const_cast<QGLContext *>(shares.at(0));
            }
        }
    }

private:
    QGLContext *ctx;

    QGL2PaintEngineExPrivate *pex;

    GLuint m_texture;
    GLuint m_fbo;

    int m_width;
    int m_height;

    QGLShaderProgram *m_program;
};

QGLTextureGlyphCache::QGLTextureGlyphCache(QGLContext *context, QFontEngineGlyphCache::Type type, const QTransform &matrix)
    : QTextureGlyphCache(type, matrix)
    , ctx(context)
    , m_width(0)
    , m_height(0)
{
    glGenFramebuffers(1, &m_fbo);
    connect(QGLSignalProxy::instance(), SIGNAL(aboutToDestroyContext(const QGLContext *)),
            SLOT(contextDestroyed(const QGLContext *)));
}

QGLTextureGlyphCache::~QGLTextureGlyphCache()
{
    if (ctx) {
        QGLContext *oldContext = const_cast<QGLContext *>(QGLContext::currentContext());
        if (oldContext != ctx)
            ctx->makeCurrent();
        glDeleteFramebuffers(1, &m_fbo);

        if (m_width || m_height)
            glDeleteTextures(1, &m_texture);
        if (oldContext && oldContext != ctx)
            oldContext->makeCurrent();
    }
}

void QGLTextureGlyphCache::createTextureData(int width, int height)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    m_width = width;
    m_height = height;

    QVarLengthArray<uchar> data(width * height);
    for (int i = 0; i < width * height; ++i)
        data[i] = 0;

    if (m_type == QFontEngineGlyphCache::Raster_RGBMask)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &data[0]);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &data[0]);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void QGLTextureGlyphCache::resizeTextureData(int width, int height)
{
    // ### the QTextureGlyphCache API needs to be reworked to allow
    // ### resizeTextureData to fail

    int oldWidth = m_width;
    int oldHeight = m_height;

    GLuint oldTexture = m_texture;
    createTextureData(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_fbo);

    GLuint colorBuffer;
    glGenRenderbuffers(1, &colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, colorBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_RGBA, oldWidth, oldHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_RENDERBUFFER_EXT, colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);

    glActiveTexture(GL_TEXTURE0 + QT_IMAGE_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D, oldTexture);

    pex->transferMode(BrushDrawingMode);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

    glViewport(0, 0, oldWidth, oldHeight);

    float vertexCoordinateArray[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
    float textureCoordinateArray[] = { 0, 0, 1, 0, 1, 1, 0, 1 };

    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glEnableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);

    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, vertexCoordinateArray);
    glVertexAttribPointer(QT_TEXTURE_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, textureCoordinateArray);

    pex->shaderManager->blitProgram()->enable();
    pex->shaderManager->blitProgram()->setUniformValue("imageTexture", QT_IMAGE_TEXTURE_UNIT);
    pex->shaderManager->setDirty();

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glDisableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, oldWidth, oldHeight);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_RENDERBUFFER_EXT, 0);
    glDeleteRenderbuffers(1, &colorBuffer);
    glDeleteTextures(1, &oldTexture);

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, ctx->d_ptr->current_fbo);

    glViewport(0, 0, pex->width, pex->height);
    pex->updateDepthScissorTest();
}

void QGLTextureGlyphCache::fillTexture(const Coord &c, glyph_t glyph)
{
    QImage mask = textureMapForGlyph(glyph);

    const uint maskWidth = mask.width();
    const uint maskHeight = mask.height();

    glBindTexture(GL_TEXTURE_2D, m_texture);
    if (mask.format() == QImage::Format_RGB32) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, c.x, m_height - c.y, maskWidth, maskHeight, GL_BGRA, GL_UNSIGNED_BYTE, mask.bits());
    } else {
        // If the width of the uploaded data is not a multiple of four bytes, we get some garbage
        // in the glyph cache, probably because of a driver bug.
        // Convert to ARGB32 to get a multiple of 4 bytes per line.
        mask = mask.convertToFormat(QImage::Format_ARGB32);
        glTexSubImage2D(GL_TEXTURE_2D, 0, c.x, c.y, maskWidth, maskHeight, GL_BGRA, GL_UNSIGNED_BYTE, mask.bits());
    }
}

extern QImage qt_imageForBrush(int brushStyle, bool invert);

////////////////////////////////// Private Methods //////////////////////////////////////////

QGL2PaintEngineExPrivate::~QGL2PaintEngineExPrivate()
{
}

void QGL2PaintEngineExPrivate::updateTextureFilter(GLenum target, GLenum wrapMode, bool smoothPixmapTransform, GLuint id)
{
//    glActiveTexture(GL_TEXTURE0 + QT_BRUSH_TEXTURE_UNIT); //### Is it always this texture unit?
    if (id != GLuint(-1) && id == lastTexture)
        return;

    lastTexture = id;

    if (smoothPixmapTransform) {
        glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    glTexParameterf(target, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameterf(target, GL_TEXTURE_WRAP_T, wrapMode);
}


QColor QGL2PaintEngineExPrivate::premultiplyColor(QColor c, GLfloat opacity)
{
    qreal alpha = c.alphaF() * opacity;
    c.setRedF(c.redF() * alpha);
    c.setGreenF(c.greenF() * alpha);
    c.setBlueF(c.blueF() * alpha);
    return c;
}


void QGL2PaintEngineExPrivate::setBrush(const QBrush* brush)
{
    currentBrush = brush;
    brushTextureDirty = true;
    brushUniformsDirty = true;
    if (currentBrush->style() == Qt::TexturePattern
        && qHasPixmapTexture(*brush) && brush->texture().isQBitmap())
    {
        shaderManager->setSrcPixelType(QGLEngineShaderManager::TextureSrcWithPattern);
    } else {
        shaderManager->setSrcPixelType(currentBrush->style());
    }
    shaderManager->optimiseForBrushTransform(currentBrush->transform());
}


// Unless this gets used elsewhere, it's probably best to merge it into fillStencilWithVertexArray
void QGL2PaintEngineExPrivate::useSimpleShader()
{
    shaderManager->simpleProgram()->enable();
    shaderManager->setDirty();

    if (matrixDirty)
        updateMatrix();

    if (simpleShaderMatrixUniformDirty) {
        shaderManager->simpleProgram()->setUniformValue("pmvMatrix", pmvMatrix);
        simpleShaderMatrixUniformDirty = false;
    }

    if (simpleShaderDepthUniformDirty) {
        shaderManager->simpleProgram()->setUniformValue("depth", (GLfloat)q->state()->currentDepth);
        simpleShaderDepthUniformDirty = false;
    }
}

void QGL2PaintEngineExPrivate::updateBrushTexture()
{
//     qDebug("QGL2PaintEngineExPrivate::updateBrushTexture()");
    Qt::BrushStyle style = currentBrush->style();

    if ( (style >= Qt::Dense1Pattern) && (style <= Qt::DiagCrossPattern) ) {
        // Get the image data for the pattern
        QImage texImage = qt_imageForBrush(style, false);

        glActiveTexture(GL_TEXTURE0 + QT_BRUSH_TEXTURE_UNIT);
        ctx->d_func()->bindTexture(texImage, GL_TEXTURE_2D, GL_RGBA, true);
        updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, true);
    }
    else if (style >= Qt::LinearGradientPattern && style <= Qt::ConicalGradientPattern) {
        // Gradiant brush: All the gradiants use the same texture

        const QGradient* g = currentBrush->gradient();

        // We apply global opacity in the fragment shaders, so we always pass 1.0
        // for opacity to the cache.
        GLuint texId = QGL2GradientCache::cacheForContext(ctx)->getBuffer(*g, 1.0);

        glActiveTexture(GL_TEXTURE0 + QT_BRUSH_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, texId);

        if (g->spread() == QGradient::RepeatSpread || g->type() == QGradient::ConicalGradient)
            updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, true);
        else if (g->spread() == QGradient::ReflectSpread)
            updateTextureFilter(GL_TEXTURE_2D, GL_MIRRORED_REPEAT_IBM, true);
        else
            updateTextureFilter(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, true);
    }
    else if (style == Qt::TexturePattern) {
        const QPixmap& texPixmap = currentBrush->texture();

        glActiveTexture(GL_TEXTURE0 + QT_BRUSH_TEXTURE_UNIT);
        ctx->d_func()->bindTexture(texPixmap, GL_TEXTURE_2D, GL_RGBA, true);
        updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, true);
    }
    brushTextureDirty = false;
}


void QGL2PaintEngineExPrivate::updateBrushUniforms()
{
//     qDebug("QGL2PaintEngineExPrivate::updateBrushUniforms()");
    Qt::BrushStyle style = currentBrush->style();

    if (style == Qt::NoBrush)
        return;

    QTransform brushQTransform = currentBrush->transform();

    if (style == Qt::SolidPattern) {
        QColor col = premultiplyColor(currentBrush->color(), (GLfloat)q->state()->opacity);
        shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::FragmentColor), col);
    }
    else {
        // All other brushes have a transform and thus need the translation point:
        QPointF translationPoint;

        if (style <= Qt::DiagCrossPattern) {
            translationPoint = q->state()->brushOrigin;

            QColor col = premultiplyColor(currentBrush->color(), (GLfloat)q->state()->opacity);

            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::PatternColor), col);

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::HalfViewportSize), halfViewportSize);
        }
        else if (style == Qt::LinearGradientPattern) {
            const QLinearGradient *g = static_cast<const QLinearGradient *>(currentBrush->gradient());

            QPointF realStart = g->start();
            QPointF realFinal = g->finalStop();
            translationPoint = realStart;

            QPointF l = realFinal - realStart;

            QVector3D linearData(
                l.x(),
                l.y(),
                1.0f / (l.x() * l.x() + l.y() * l.y())
            );

            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::LinearData), linearData);

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::HalfViewportSize), halfViewportSize);
        }
        else if (style == Qt::ConicalGradientPattern) {
            const QConicalGradient *g = static_cast<const QConicalGradient *>(currentBrush->gradient());
            translationPoint   = g->center();

            GLfloat angle = -(g->angle() * 2 * Q_PI) / 360.0;

            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::Angle), angle);

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::HalfViewportSize), halfViewportSize);
        }
        else if (style == Qt::RadialGradientPattern) {
            const QRadialGradient *g = static_cast<const QRadialGradient *>(currentBrush->gradient());
            QPointF realCenter = g->center();
            QPointF realFocal  = g->focalPoint();
            qreal   realRadius = g->radius();
            translationPoint   = realFocal;

            QPointF fmp = realCenter - realFocal;
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::Fmp), fmp);

            GLfloat fmp2_m_radius2 = -fmp.x() * fmp.x() - fmp.y() * fmp.y() + realRadius*realRadius;
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::Fmp2MRadius2), fmp2_m_radius2);
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::Inverse2Fmp2MRadius2),
                                                             GLfloat(1.0 / (2.0*fmp2_m_radius2)));

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::HalfViewportSize), halfViewportSize);
        }
        else if (style == Qt::TexturePattern) {
            translationPoint = q->state()->brushOrigin;

            const QPixmap& texPixmap = currentBrush->texture();

            if (qHasPixmapTexture(*currentBrush) && currentBrush->texture().isQBitmap()) {
                QColor col = premultiplyColor(currentBrush->color(), (GLfloat)q->state()->opacity);
                shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::PatternColor), col);
            }

            QSizeF invertedTextureSize( 1.0 / texPixmap.width(), 1.0 / texPixmap.height() );
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::InvertedTextureSize), invertedTextureSize);

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::HalfViewportSize), halfViewportSize);
        }
        else
            qWarning("QGL2PaintEngineEx: Unimplemented fill style");

        QTransform translate(1, 0, 0, 1, -translationPoint.x(), -translationPoint.y());
        QTransform gl_to_qt(1, 0, 0, -1, 0, height);
        QTransform inv_matrix = gl_to_qt * (brushQTransform * q->state()->matrix).inverted() * translate;

        shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::BrushTransform), inv_matrix);
        shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::BrushTexture), QT_BRUSH_TEXTURE_UNIT);
    }
    brushUniformsDirty = false;
}


// This assumes the shader manager has already setup the correct shader program
void QGL2PaintEngineExPrivate::updateMatrix()
{
//     qDebug("QGL2PaintEngineExPrivate::updateMatrix()");

    // We setup the Projection matrix to be the equivilant of glOrtho(0, w, h, 0, -1, 1):
    GLfloat P[4][4] = {
        {2.0/width,  0.0,        0.0, -1.0},
        {0.0,       -2.0/height, 0.0,  1.0},
        {0.0,        0.0,       -1.0,  0.0},
        {0.0,        0.0,        0.0,  1.0}
    };

    const QTransform& transform = q->state()->matrix;

    if (mode == TextDrawingMode) {
        // Text drawing mode is only used for non-scaling transforms
        for (int row = 0; row < 4; ++row)
            for (int col = 0; col < 4; ++col)
                pmvMatrix[col][row] = P[row][col];

        pmvMatrix[3][0] += P[0][0] * qRound(transform.dx());
        pmvMatrix[3][1] += P[1][1] * qRound(transform.dy());

        inverseScale = 1;
    } else {
        // Use the (3x3) transform for the Model~View matrix:
        GLfloat MV[4][4] = {
            {transform.m11(), transform.m21(), 0.0, transform.dx()},
            {transform.m12(), transform.m22(), 0.0, transform.dy()},
            {0.0,             0.0,             1.0, 0.0},
            {transform.m13(), transform.m23(), 0.0, transform.m33()}
        };

        // NOTE: OpenGL ES works with column-major matrices, so when we multiply the matrices,
        //       we also transpose them ready for GL.
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                pmvMatrix[col][row] = 0.0;

                // P[row][n] is 0.0 for n < row
                for (int n = row; n < 4; ++n)
                    pmvMatrix[col][row] += P[row][n] * MV[n][col];
            }
        }

        // 1/10000 == 0.0001, so we have good enough res to cover curves
        // that span the entire widget...
        inverseScale = qMax(1 / qMax( qMax(qAbs(transform.m11()), qAbs(transform.m22())),
                    qMax(qAbs(transform.m12()), qAbs(transform.m21())) ),
                qreal(0.0001));
    }

    matrixDirty = false;

    // The actual data has been updated so both shader program's uniforms need updating
    simpleShaderMatrixUniformDirty = true;
    shaderMatrixUniformDirty = true;
}


void QGL2PaintEngineExPrivate::updateCompositionMode()
{
    // NOTE: The entire paint engine works on pre-multiplied data - which is why some of these
    //       composition modes look odd.
//     qDebug() << "QGL2PaintEngineExPrivate::updateCompositionMode() - Setting GL composition mode for " << q->state()->composition_mode;
    switch(q->state()->composition_mode) {
    case QPainter::CompositionMode_SourceOver:
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_DestinationOver:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
        break;
    case QPainter::CompositionMode_Clear:
        glBlendFunc(GL_ZERO, GL_ZERO);
        break;
    case QPainter::CompositionMode_Source:
        glBlendFunc(GL_ONE, GL_ZERO);
        break;
    case QPainter::CompositionMode_Destination:
        glBlendFunc(GL_ZERO, GL_ONE);
        break;
    case QPainter::CompositionMode_SourceIn:
        glBlendFunc(GL_DST_ALPHA, GL_ZERO);
        break;
    case QPainter::CompositionMode_DestinationIn:
        glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_SourceOut:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
        break;
    case QPainter::CompositionMode_DestinationOut:
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_SourceAtop:
        glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_DestinationAtop:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_Xor:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_Plus:
        glBlendFunc(GL_ONE, GL_ONE);
        break;
    default:
        qWarning("Unsupported composition mode");
        break;
    }

    compositionModeDirty = false;
}

static inline void setCoords(GLfloat *coords, const QGLRect &rect)
{
    coords[0] = rect.left;
    coords[1] = rect.top;
    coords[2] = rect.right;
    coords[3] = rect.top;
    coords[4] = rect.right;
    coords[5] = rect.bottom;
    coords[6] = rect.left;
    coords[7] = rect.bottom;
}

void QGL2PaintEngineExPrivate::drawTexture(const QGLRect& dest, const QGLRect& src, const QSize &textureSize, bool opaque, bool pattern)
{
    // Setup for texture drawing
    shaderManager->setSrcPixelType(pattern ? QGLEngineShaderManager::PatternSrc : QGLEngineShaderManager::ImageSrc);
    shaderManager->setTextureCoordsEnabled(true);
    if (prepareForDraw(opaque))
        shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::ImageTexture), QT_IMAGE_TEXTURE_UNIT);

    if (pattern) {
        QColor col = premultiplyColor(q->state()->pen.color(), (GLfloat)q->state()->opacity);
        shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::PatternColor), col);
    }

    GLfloat dx = 1.0 / textureSize.width();
    GLfloat dy = 1.0 / textureSize.height();

    QGLRect srcTextureRect(src.left*dx, 1.0 - src.top*dy, src.right*dx, 1.0 - src.bottom*dy);

    setCoords(staticVertexCoordinateArray, dest);
    setCoords(staticTextureCoordinateArray, srcTextureRect);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void QGL2PaintEngineEx::sync()
{
    Q_D(QGL2PaintEngineEx);
    ensureActive();
    d->transferMode(BrushDrawingMode);

    QGLContext *ctx = d->ctx;
    glUseProgram(0);

#ifndef QT_OPENGL_ES_2
    // be nice to people who mix OpenGL 1.x code with QPainter commands
    // by setting modelview and projection matrices to mirror the GL 1
    // paint engine
    const QTransform& mtx = state()->matrix;

    float mv_matrix[4][4] =
    {
        { mtx.m11(), mtx.m12(),     0, mtx.m13() },
        { mtx.m21(), mtx.m22(),     0, mtx.m23() },
        {         0,         0,     1,         0 },
        {  mtx.dx(),  mtx.dy(),     0, mtx.m33() }
    };

    const QSize sz = d->drawable.size();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, sz.width(), sz.height(), 0, -999999, 999999);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&mv_matrix[0][0]);
#endif

    d->lastTexture = GLuint(-1);

    glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glClearDepth(1);

    d->needsSync = true;
}

void QGL2PaintEngineExPrivate::transferMode(EngineMode newMode)
{
    if (newMode == mode)
        return;

    if (mode == TextDrawingMode || mode == ImageDrawingMode) {
        glDisableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);
        glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);

        lastTexture = GLuint(-1);
    }

    if (mode == TextDrawingMode)
        matrixDirty = true;

    if (newMode == TextDrawingMode) {
        glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
        glEnableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);

        glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, vertexCoordinateArray.data());
        glVertexAttribPointer(QT_TEXTURE_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, textureCoordinateArray.data());

        matrixDirty = true;
    }

    if (newMode == ImageDrawingMode) {
        glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
        glEnableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);

        glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, staticVertexCoordinateArray);
        glVertexAttribPointer(QT_TEXTURE_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, staticTextureCoordinateArray);
    }

    // This needs to change when we implement high-quality anti-aliasing...
    if (newMode != TextDrawingMode)
        shaderManager->setMaskType(QGLEngineShaderManager::NoMask);

    mode = newMode;
}

void QGL2PaintEngineExPrivate::drawOutline(const QVectorPath& path)
{
    transferMode(BrushDrawingMode);

    // Might need to call updateMatrix to re-calculate inverseScale
    if (matrixDirty)
        updateMatrix();

    vertexCoordinateArray.clear();
    vertexCoordinateArray.addPath(path, inverseScale);

    if (path.hasImplicitClose()) {
        // Close the path's outline
        vertexCoordinateArray.lineToArray(path.points()[0], path.points()[1]);
        vertexCoordinateArray.stops().last() += 1;
    }

    prepareForDraw(currentBrush->isOpaque());
    drawVertexArrays(vertexCoordinateArray, GL_LINE_STRIP);
}


// Assumes everything is configured for the brush you want to use
void QGL2PaintEngineExPrivate::fill(const QVectorPath& path)
{
    transferMode(BrushDrawingMode);

    // Might need to call updateMatrix to re-calculate inverseScale
    if (matrixDirty)
        updateMatrix();

    const QPointF* const points = reinterpret_cast<const QPointF*>(path.points());

    // Check to see if there's any hints
    if (path.shape() == QVectorPath::RectangleHint) {
        QGLRect rect(points[0].x(), points[0].y(), points[2].x(), points[2].y());
        prepareForDraw(currentBrush->isOpaque());

        composite(rect);
    }
    else if (path.shape() == QVectorPath::EllipseHint) {
        vertexCoordinateArray.clear();
        vertexCoordinateArray.addPath(path, inverseScale);
        prepareForDraw(currentBrush->isOpaque());
        drawVertexArrays(vertexCoordinateArray, GL_TRIANGLE_FAN);
    }
    else {
        // The path is too complicated & needs the stencil technique
        vertexCoordinateArray.clear();
        vertexCoordinateArray.addPath(path, inverseScale);

        fillStencilWithVertexArray(vertexCoordinateArray, path.hasWindingFill());

        // Stencil the brush onto the dest buffer
        glStencilFunc(GL_NOTEQUAL, 0, 0xFFFF); // Pass if stencil buff value != 0
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

        glEnable(GL_STENCIL_TEST);
        prepareForDraw(currentBrush->isOpaque());
        composite(vertexCoordinateArray.boundingRect());
        glDisable(GL_STENCIL_TEST);

        glStencilMask(0);
    }
}


void QGL2PaintEngineExPrivate::fillStencilWithVertexArray(QGL2PEXVertexArray& vertexArray, bool useWindingFill)
{
//     qDebug("QGL2PaintEngineExPrivate::fillStencilWithVertexArray()");
    glStencilMask(0xFFFF); // Enable stencil writes

    if (stencilBufferDirty) {
        // Clear the stencil buffer to zeros
        glDisable(GL_STENCIL_TEST);
        glClearStencil(0); // Clear to zero
        glClear(GL_STENCIL_BUFFER_BIT);
        stencilBufferDirty = false;
    }

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writes
    glStencilFunc(GL_ALWAYS, 0, 0xFFFF); // Always pass the stencil test

    // Setup the stencil op:
    if (useWindingFill) {
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP); // Inc. for front-facing triangle
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP); //Dec. for back-facing "holes"
    } else
        glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT); // Simply invert the stencil bit

    // No point in using a fancy gradient shader for writing into the stencil buffer!
    useSimpleShader();

    glEnable(GL_STENCIL_TEST); // For some reason, this has to happen _after_ the simple shader is use()'d
    glDisable(GL_BLEND);

    // Draw the vertecies into the stencil buffer:
    drawVertexArrays(vertexArray, GL_TRIANGLE_FAN);

    // Enable color writes & disable stencil writes
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

bool QGL2PaintEngineExPrivate::prepareForDraw(bool srcPixelsAreOpaque)
{
    if (brushTextureDirty && mode != ImageDrawingMode)
        updateBrushTexture();

    if (compositionModeDirty)
        updateCompositionMode();

    if (matrixDirty)
        updateMatrix();

    const bool stateHasOpacity = q->state()->opacity < 0.99f;
    if (q->state()->composition_mode == QPainter::CompositionMode_Source
        || (q->state()->composition_mode == QPainter::CompositionMode_SourceOver
            && srcPixelsAreOpaque && !stateHasOpacity))
    {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
    }

    bool useGlobalOpacityUniform = stateHasOpacity;
    if (stateHasOpacity && (mode != ImageDrawingMode)) {
        // Using a brush
        bool brushIsPattern = (currentBrush->style() >= Qt::Dense1Pattern) &&
                              (currentBrush->style() <= Qt::DiagCrossPattern);

        if ((currentBrush->style() == Qt::SolidPattern) || brushIsPattern)
            useGlobalOpacityUniform = false; // Global opacity handled by srcPixel shader
    }
    shaderManager->setUseGlobalOpacity(useGlobalOpacityUniform);

    bool changed = shaderManager->useCorrectShaderProg();
    // If the shader program needs changing, we change it and mark all uniforms as dirty
    if (changed) {
        // The shader program has changed so mark all uniforms as dirty:
        brushUniformsDirty = true;
        shaderMatrixUniformDirty = true;
        depthUniformDirty = true;
        opacityUniformDirty = true;
    }

    if (brushUniformsDirty && mode != ImageDrawingMode)
        updateBrushUniforms();

    if (shaderMatrixUniformDirty) {
        shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::PmvMatrix), pmvMatrix);
        shaderMatrixUniformDirty = false;
    }

    if (depthUniformDirty) {
        shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::Depth), (GLfloat)q->state()->currentDepth);
        depthUniformDirty = false;
    }

    if (useGlobalOpacityUniform && opacityUniformDirty) {
        shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::GlobalOpacity), (GLfloat)q->state()->opacity);
        opacityUniformDirty = false;
    }

    return changed;
}

void QGL2PaintEngineExPrivate::composite(const QGLRect& boundingRect)
{
    // Setup a vertex array for the bounding rect:
    GLfloat rectVerts[] = {
        boundingRect.left, boundingRect.top,
        boundingRect.left, boundingRect.bottom,
        boundingRect.right, boundingRect.bottom,
        boundingRect.right, boundingRect.top
    };

    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, rectVerts);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
}

// Draws the vertex array as a set of <vertexArrayStops.size()> triangle fans.
void QGL2PaintEngineExPrivate::drawVertexArrays(QGL2PEXVertexArray& vertexArray, GLenum primitive)
{
    // Now setup the pointer to the vertex array:
    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, vertexArray.data());

    int previousStop = 0;
    foreach(int stop, vertexArray.stops()) {
/*
        qDebug("Drawing triangle fan for vertecies %d -> %d:", previousStop, stop-1);
        for (int i=previousStop; i<stop; ++i)
            qDebug("   %02d: [%.2f, %.2f]", i, vertexArray.data()[i].x, vertexArray.data()[i].y);
*/
        glDrawArrays(primitive, previousStop, stop - previousStop);
        previousStop = stop;
    }
    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
}





/////////////////////////////////// Public Methods //////////////////////////////////////////

QGL2PaintEngineEx::QGL2PaintEngineEx()
    : QPaintEngineEx(*(new QGL2PaintEngineExPrivate(this)))
{
}

QGL2PaintEngineEx::~QGL2PaintEngineEx()
{
}

void QGL2PaintEngineEx::fill(const QVectorPath &path, const QBrush &brush)
{
    Q_D(QGL2PaintEngineEx);

    if (brush.style() == Qt::NoBrush)
        return;

    ensureActive();
    d->setBrush(&brush);
    d->fill(path);
    d->setBrush(&(state()->brush)); // reset back to the state's brush
}

void QGL2PaintEngineEx::stroke(const QVectorPath &path, const QPen &pen)
{
    Q_D(QGL2PaintEngineEx);

    if (pen.style() == Qt::NoPen)
        return;

    ensureActive();

    if ( (pen.isCosmetic() && (pen.style() == Qt::SolidLine)) && (pen.widthF() < 2.5f) )
    {
        // We only handle solid, cosmetic pens with a width of 1 pixel
        const QBrush& brush = pen.brush();
        d->setBrush(&brush);

        if (pen.widthF() < 0.01f)
            glLineWidth(1.0);
        else
            glLineWidth(pen.widthF());

        d->drawOutline(path);
        d->setBrush(&(state()->brush));
    } else
        return QPaintEngineEx::stroke(path, pen);
}

void QGL2PaintEngineEx::penChanged()
{
//    qDebug("QGL2PaintEngineEx::penChanged() not implemented!");
}


void QGL2PaintEngineEx::brushChanged()
{
//    qDebug("QGL2PaintEngineEx::brushChanged()");
    Q_D(QGL2PaintEngineEx);
    d->setBrush(&(state()->brush));
}

void QGL2PaintEngineEx::brushOriginChanged()
{
//    qDebug("QGL2PaintEngineEx::brushOriginChanged()");
    Q_D(QGL2PaintEngineEx);
    d->brushUniformsDirty = true;
}

void QGL2PaintEngineEx::opacityChanged()
{
//    qDebug("QGL2PaintEngineEx::opacityChanged()");
    Q_D(QGL2PaintEngineEx);

    Q_ASSERT(d->shaderManager);
    d->brushUniformsDirty = true;
    d->opacityUniformDirty = true;
}

void QGL2PaintEngineEx::compositionModeChanged()
{
//     qDebug("QGL2PaintEngineEx::compositionModeChanged()");
    Q_D(QGL2PaintEngineEx);
    d->compositionModeDirty = true;
}

void QGL2PaintEngineEx::renderHintsChanged()
{
#if !defined(QT_OPENGL_ES_2)
    if ((state()->renderHints & QPainter::Antialiasing)
        || (state()->renderHints & QPainter::HighQualityAntialiasing))
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);
#endif

    Q_D(QGL2PaintEngineEx);
    d->lastTexture = GLuint(-1);
//    qDebug("QGL2PaintEngineEx::renderHintsChanged() not implemented!");
}

void QGL2PaintEngineEx::transformChanged()
{
    Q_D(QGL2PaintEngineEx);
    d->matrixDirty = true;
}


void QGL2PaintEngineEx::drawPixmap(const QRectF& dest, const QPixmap & pixmap, const QRectF & src)
{
    Q_D(QGL2PaintEngineEx);
    ensureActive();
    d->transferMode(ImageDrawingMode);

    QGLContext *ctx = d->ctx;
    glActiveTexture(GL_TEXTURE0 + QT_IMAGE_TEXTURE_UNIT);
    GLuint id = ctx->d_func()->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, true);

    bool isBitmap = pixmap.isQBitmap();
    bool isOpaque = !isBitmap && !pixmap.hasAlphaChannel();

    d->updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT,
                           state()->renderHints & QPainter::SmoothPixmapTransform, id);
    d->drawTexture(dest, src, pixmap.size(), isOpaque, isBitmap);
}

void QGL2PaintEngineEx::drawImage(const QRectF& dest, const QImage& image, const QRectF& src,
                        Qt::ImageConversionFlags)
{
    Q_D(QGL2PaintEngineEx);
    ensureActive();
    d->transferMode(ImageDrawingMode);

    QGLContext *ctx = d->ctx;
    glActiveTexture(GL_TEXTURE0 + QT_IMAGE_TEXTURE_UNIT);
    GLuint id = ctx->d_func()->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, true);

    d->updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT,
                           state()->renderHints & QPainter::SmoothPixmapTransform, id);
    d->drawTexture(dest, src, image.size(), !image.hasAlphaChannel());
}

void QGL2PaintEngineEx::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
    Q_D(QGL2PaintEngineEx);

    ensureActive();
    QOpenGL2PaintEngineState *s = state();

    const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);

    bool drawCached = true;

    if (s->matrix.type() > QTransform::TxTranslate)
        drawCached = false;

    // don't try to cache huge fonts
    if (ti.fontEngine->fontDef.pixelSize * qSqrt(s->matrix.determinant()) >= 64)
        drawCached = false;

    if (drawCached) {
        d->drawCachedGlyphs(p, ti);
        return;
    }

    QPaintEngineEx::drawTextItem(p, ti);
}

void QGL2PaintEngineExPrivate::drawCachedGlyphs(const QPointF &p, const QTextItemInt &ti)
{
    Q_Q(QGL2PaintEngineEx);
    QOpenGL2PaintEngineState *s = q->state();

    QVarLengthArray<QFixedPoint> positions;
    QVarLengthArray<glyph_t> glyphs;
    QTransform matrix = QTransform::fromTranslate(p.x(), p.y());
    ti.fontEngine->getGlyphPositions(ti.glyphs, matrix, ti.flags, glyphs, positions);

    QFontEngineGlyphCache::Type glyphType = ti.fontEngine->glyphFormat >= 0
        ? QFontEngineGlyphCache::Type(ti.fontEngine->glyphFormat)
        : QFontEngineGlyphCache::Raster_A8;

    QGLTextureGlyphCache *cache =
        (QGLTextureGlyphCache *) ti.fontEngine->glyphCache(ctx, s->matrix);
    if (!cache) {
        cache = new QGLTextureGlyphCache(ctx, glyphType, s->matrix);
        ti.fontEngine->setGlyphCache(ctx, cache);
    }

    cache->setPaintEnginePrivate(this);
    cache->populate(ti, glyphs, positions);

    if (cache->width() == 0 || cache->height() == 0)
        return;

    transferMode(TextDrawingMode);

    if (glyphType == QFontEngineGlyphCache::Raster_A8)
        shaderManager->setMaskType(QGLEngineShaderManager::PixelMask);
    else if (glyphType == QFontEngineGlyphCache::Raster_RGBMask)
        shaderManager->setMaskType(QGLEngineShaderManager::SubPixelMask);
    //### TODO: Gamma correction
    shaderManager->setTextureCoordsEnabled(true);

    int margin = cache->glyphMargin();

    GLfloat dx = 1.0 / cache->width();
    GLfloat dy = 1.0 / cache->height();

    QGLPoint *oldVertexCoordinateDataPtr = vertexCoordinateArray.data();
    QGLPoint *oldTextureCoordinateDataPtr = textureCoordinateArray.data();

    vertexCoordinateArray.clear();
    textureCoordinateArray.clear();

    for (int i=0; i<glyphs.size(); ++i) {
        const QTextureGlyphCache::Coord &c = cache->coords.value(glyphs[i]);
        int x = positions[i].x.toInt() + c.baseLineX - margin;
        int y = positions[i].y.toInt() - c.baseLineY - margin;

        vertexCoordinateArray.addRect(QRectF(x, y, c.w, c.h));
        textureCoordinateArray.addRect(QRectF(c.x*dx, c.y*dy, c.w * dx, c.h * dy));
    }

    glActiveTexture(GL_TEXTURE0 + QT_MASK_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D, cache->texture());
    updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, false);

    QBrush pensBrush = q->state()->pen.brush();
    setBrush(&pensBrush);

    prepareForDraw(false); // Text always causes src pixels to be transparent

    shaderManager->currentProgram()->setUniformValue(location(QGLEngineShaderManager::MaskTexture), QT_MASK_TEXTURE_UNIT);

    if (vertexCoordinateArray.data() != oldVertexCoordinateDataPtr)
        glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, vertexCoordinateArray.data());
    if (textureCoordinateArray.data() != oldTextureCoordinateDataPtr)
        glVertexAttribPointer(QT_TEXTURE_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, textureCoordinateArray.data());

    glDrawArrays(GL_TRIANGLES, 0, 6 * glyphs.size());

    setBrush(&(q->state()->brush)); //###
}

bool QGL2PaintEngineEx::begin(QPaintDevice *pdev)
{
    Q_D(QGL2PaintEngineEx);

//     qDebug("QGL2PaintEngineEx::begin()");
    d->drawable.setDevice(pdev);
    d->ctx = d->drawable.context();

    if (d->ctx->d_ptr->active_engine) {
        QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(d->ctx->d_ptr->active_engine);
        QGL2PaintEngineExPrivate *p = static_cast<QGL2PaintEngineExPrivate *>(engine->d_ptr);
        p->transferMode(BrushDrawingMode);
        p->drawable.doneCurrent();
    }

    d->ctx->d_ptr->active_engine = this;
    d->last_created_state = 0;

    d->drawable.makeCurrent();
    QSize sz = d->drawable.size();
    d->width = sz.width();
    d->height = sz.height();
    d->mode = BrushDrawingMode;

#if !defined(QT_OPENGL_ES_2)
    qt_resolve_version_2_0_functions(d->ctx);
#endif

    d->shaderManager = QGLEngineShaderManager::managerForContext(d->ctx);
    d->shaderManager->setDirty();

    glViewport(0, 0, d->width, d->height);

//     glClearColor(0.0, 1.0, 0.0, 1.0);
//     glClear(GL_COLOR_BUFFER_BIT);
//     d->ctx->swapBuffers();
//     qDebug("You should see green now");
//     sleep(5);

    d->brushTextureDirty = true;
    d->brushUniformsDirty = true;
    d->matrixDirty = true;
    d->compositionModeDirty = true;
    d->stencilBufferDirty = true;
    d->simpleShaderDepthUniformDirty = true;
    d->depthUniformDirty = true;
    d->opacityUniformDirty = true;
    d->needsSync = false;

    d->use_system_clip = !systemClip().isEmpty();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(false);

#if !defined(QT_OPENGL_ES_2)
    glDisable(GL_MULTISAMPLE);
#endif

    QGLPixmapData *source = d->drawable.copyOnBegin();
    if (d->drawable.context()->d_func()->clear_on_painter_begin && d->drawable.autoFillBackground()) {
        if (d->drawable.hasTransparentBackground())
            glClearColor(0.0, 0.0, 0.0, 0.0);
        else {
            const QColor &c = d->drawable.backgroundColor();
            float alpha = c.alphaF();
            glClearColor(c.redF() * alpha, c.greenF() * alpha, c.blueF() * alpha, alpha);
        }
        glClear(GL_COLOR_BUFFER_BIT);
    } else if (source) {
        QGLContext *ctx = d->ctx;

        d->transferMode(ImageDrawingMode);

        glActiveTexture(GL_TEXTURE0 + QT_IMAGE_TEXTURE_UNIT);
        source->bind(false);

        QRect rect(0, 0, source->width(), source->height());
        d->updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, false);
        d->drawTexture(QRectF(rect), QRectF(rect), rect.size(), true);
    }

    d->systemStateChanged();
    return true;
}

bool QGL2PaintEngineEx::end()
{
    Q_D(QGL2PaintEngineEx);
    QGLContext *ctx = d->ctx;
    if (ctx->d_ptr->active_engine != this) {
        QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(ctx->d_ptr->active_engine);
        if (engine && engine->isActive()) {
            QGL2PaintEngineExPrivate *p = static_cast<QGL2PaintEngineExPrivate *>(engine->d_ptr);
            p->transferMode(BrushDrawingMode);
            p->drawable.doneCurrent();
        }
        d->drawable.makeCurrent();
    }

    glUseProgram(0);
    d->transferMode(BrushDrawingMode);
    d->drawable.swapBuffers();
    d->drawable.doneCurrent();
    d->ctx->d_ptr->active_engine = 0;

    return false;
}

void QGL2PaintEngineEx::ensureActive()
{
    Q_D(QGL2PaintEngineEx);
    QGLContext *ctx = d->ctx;

    if (isActive() && ctx->d_ptr->active_engine != this) {
        QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(ctx->d_ptr->active_engine);
        if (engine && engine->isActive()) {
            QGL2PaintEngineExPrivate *p = static_cast<QGL2PaintEngineExPrivate *>(engine->d_ptr);
            p->transferMode(BrushDrawingMode);
            p->drawable.doneCurrent();
        }
        d->drawable.makeCurrent();

        ctx->d_ptr->active_engine = this;

        d->needsSync = true;
    }

    if (d->needsSync) {
        glViewport(0, 0, d->width, d->height);
        glDepthMask(false);
        glDepthFunc(GL_LEQUAL);
        setState(state());
        d->needsSync = false;
    }
}

void QGL2PaintEngineExPrivate::updateDepthScissorTest()
{
    Q_Q(QGL2PaintEngineEx);
    if (q->state()->depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if (q->state()->scissorTestEnabled)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

void QGL2PaintEngineEx::clipEnabledChanged()
{
    Q_D(QGL2PaintEngineEx);

    d->simpleShaderDepthUniformDirty = true;
    d->depthUniformDirty = true;

    if (painter()->hasClipping()) {
        d->regenerateDepthClip();
    } else {
        if (d->use_system_clip) {
            state()->currentDepth = -0.5f;
        } else {
            glDisable(GL_DEPTH_TEST);
            state()->depthTestEnabled = false;
        }
    }
}

void QGL2PaintEngineExPrivate::writeClip(const QVectorPath &path, float depth)
{
    transferMode(BrushDrawingMode);

    if (matrixDirty)
        updateMatrix();

    if (q->state()->needsDepthBufferClear) {
        glDepthMask(true);
        glClearDepth(0.5);
        glClear(GL_DEPTH_BUFFER_BIT);
        q->state()->needsDepthBufferClear = false;
        glDepthMask(false);
    }

    if (path.isEmpty())
        return;

    glDisable(GL_BLEND);
    glDepthMask(false);

    vertexCoordinateArray.clear();
    vertexCoordinateArray.addPath(path, inverseScale);

    glDepthMask(GL_FALSE);
    fillStencilWithVertexArray(vertexCoordinateArray, path.hasWindingFill());

    // Stencil the clip onto the clip buffer
    glColorMask(false, false, false, false);
    glDepthMask(true);

    shaderManager->simpleProgram()->setUniformValue("depth", depth);
    simpleShaderDepthUniformDirty = true;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glStencilFunc(GL_NOTEQUAL, 0, 0xFFFF); // Pass if stencil buff value != 0
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glEnable(GL_STENCIL_TEST);
    composite(vertexCoordinateArray.boundingRect());
    glDisable(GL_STENCIL_TEST);

    glStencilMask(0);

    glColorMask(true, true, true, true);
    glDepthMask(false);
}

void QGL2PaintEngineEx::clip(const QVectorPath &path, Qt::ClipOperation op)
{
//     qDebug("QGL2PaintEngineEx::clip()");
    Q_D(QGL2PaintEngineEx);

    if (op == Qt::ReplaceClip && !d->hasClipOperations())
        op = Qt::IntersectClip;

    if (!path.isEmpty() && op == Qt::IntersectClip && (path.hints() & QVectorPath::RectangleHint)) {
        const QPointF* const points = reinterpret_cast<const QPointF*>(path.points());
        QRectF rect(points[0], points[2]);

        if (state()->matrix.type() <= QTransform::TxScale) {
            rect = state()->matrix.mapRect(rect);

            if ((d->use_system_clip && rect.contains(d->systemClip.boundingRect()))
                || rect.contains(QRect(0, 0, d->width, d->height)))
                return;
        }
    }

    switch (op) {
    case Qt::NoClip:
        if (d->use_system_clip) {
            glEnable(GL_DEPTH_TEST);
            state()->depthTestEnabled = true;
            state()->currentDepth = -0.5;
        } else {
            glDisable(GL_DEPTH_TEST);
            state()->depthTestEnabled = false;
        }
        state()->canRestoreClip = false;
        break;
    case Qt::IntersectClip:
        state()->maxDepth = (1.0f + state()->maxDepth) * 0.5;
        d->writeClip(path, state()->maxDepth);
        state()->currentDepth = 1.5 * state()->maxDepth - 0.5f;
        state()->depthTestEnabled = true;
        break;
    case Qt::ReplaceClip:
        d->systemStateChanged();
        state()->maxDepth = 0.5f;
        glDepthFunc(GL_ALWAYS);
        d->writeClip(path, state()->maxDepth);
        state()->currentDepth = 0.25f;
        state()->canRestoreClip = false;
        state()->depthTestEnabled = true;
        break;
    case Qt::UniteClip:
        glDepthFunc(GL_ALWAYS);
        d->writeClip(path, state()->maxDepth);
        state()->canRestoreClip = false;
        state()->depthTestEnabled = true;
        break;
    }

    glDepthFunc(GL_LEQUAL);
    if (state()->depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
        d->simpleShaderDepthUniformDirty = true;
        d->depthUniformDirty = true;
    }
}

void QGL2PaintEngineExPrivate::regenerateDepthClip()
{
    systemStateChanged();
    replayClipOperations();
}

void QGL2PaintEngineExPrivate::systemStateChanged()
{
    Q_Q(QGL2PaintEngineEx);
    use_system_clip = !systemClip.isEmpty();

    glDisable(GL_DEPTH_TEST);
    q->state()->depthTestEnabled = false;
    q->state()->scissorTestEnabled = false;
    q->state()->needsDepthBufferClear = true;

    glDisable(GL_SCISSOR_TEST);

    q->state()->currentDepth = -0.5f;
    q->state()->maxDepth = 0.5f;

    if (use_system_clip) {
        QRect bounds = systemClip.boundingRect();
        if (systemClip.numRects() == 1
            && bounds == QRect(0, 0, width, height))
        {
            q->state()->needsDepthBufferClear = true;
        } else {
            glEnable(GL_SCISSOR_TEST);

            const int left = bounds.left();
            const int width = bounds.width();
            const int bottom = height - (bounds.top() + bounds.height());
            const int height = bounds.height();

            glScissor(left, bottom, width, height);

            QTransform transform = q->state()->matrix;
            q->state()->matrix = QTransform();
            q->transformChanged();

            q->state()->needsDepthBufferClear = false;

            glDepthMask(true);

            glClearDepth(0);
            glClear(GL_DEPTH_BUFFER_BIT);

            QPainterPath path;
            path.addRegion(systemClip);

            glDepthFunc(GL_ALWAYS);
            writeClip(qtVectorPathForPath(path), 0.0f);
            glDepthFunc(GL_LEQUAL);

            glEnable(GL_DEPTH_TEST);
            q->state()->depthTestEnabled = true;
            q->state()->scissorTestEnabled = true;

            q->state()->matrix = transform;
            q->transformChanged();
        }

        q->state()->currentDepth = -0.5f;
        simpleShaderDepthUniformDirty = true;
        depthUniformDirty = true;
    }
}

void QGL2PaintEngineEx::setState(QPainterState *new_state)
{
    //     qDebug("QGL2PaintEngineEx::setState()");

    Q_D(QGL2PaintEngineEx);

    QOpenGL2PaintEngineState *s = static_cast<QOpenGL2PaintEngineState *>(new_state);
    QOpenGL2PaintEngineState *old_state = state();

    QPaintEngineEx::setState(s);

    if (s == d->last_created_state) {
        d->last_created_state = 0;
        return;
    }

    renderHintsChanged();

    d->matrixDirty = true;
    d->compositionModeDirty = true;
    d->brushTextureDirty = true;
    d->brushUniformsDirty = true;
    d->simpleShaderDepthUniformDirty = true;
    d->depthUniformDirty = true;
    d->simpleShaderMatrixUniformDirty = true;
    d->shaderMatrixUniformDirty = true;
    d->opacityUniformDirty = true;

    d->shaderManager->setDirty();

    if (old_state && old_state != s && old_state->canRestoreClip) {
        d->updateDepthScissorTest();
        glDepthMask(false);
        glDepthFunc(GL_LEQUAL);
        s->maxDepth = old_state->maxDepth;
    } else {
        d->regenerateDepthClip();
    }
}

QPainterState *QGL2PaintEngineEx::createState(QPainterState *orig) const
{
    Q_D(const QGL2PaintEngineEx);

    QOpenGL2PaintEngineState *s;
    if (!orig)
        s = new QOpenGL2PaintEngineState();
    else
        s = new QOpenGL2PaintEngineState(*static_cast<QOpenGL2PaintEngineState *>(orig));

    d->last_created_state = s;
    return s;
}

QOpenGL2PaintEngineState::QOpenGL2PaintEngineState(QOpenGL2PaintEngineState &other)
    : QPainterState(other)
{
    needsDepthBufferClear = other.needsDepthBufferClear;
    depthTestEnabled = other.depthTestEnabled;
    scissorTestEnabled = other.scissorTestEnabled;
    currentDepth = other.currentDepth;
    maxDepth = other.maxDepth;
    canRestoreClip = other.canRestoreClip;
}

QOpenGL2PaintEngineState::QOpenGL2PaintEngineState()
{
    needsDepthBufferClear = true;
    depthTestEnabled = false;
    scissorTestEnabled = false;
    currentDepth = -0.5f;
    maxDepth = 0.5f;
    canRestoreClip = true;
}

QOpenGL2PaintEngineState::~QOpenGL2PaintEngineState()
{
}

QT_END_NAMESPACE

#include "qpaintengineex_opengl2.moc"
