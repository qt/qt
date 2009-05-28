/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
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

extern QImage qt_imageForBrush(int brushStyle, bool invert);

static const GLuint QT_BRUSH_TEXTURE_UNIT       = 0;
static const GLuint QT_IMAGE_TEXTURE_UNIT       = 0; //Can be the same as brush texture unit
static const GLuint QT_MASK_TEXTURE_UNIT        = 1;
static const GLuint QT_BACKGROUND_TEXTURE_UNIT  = 2;

////////////////////////////////// Private Methods //////////////////////////////////////////

QGL2PaintEngineExPrivate::~QGL2PaintEngineExPrivate()
{
    if (shaderManager) {
        delete shaderManager;
        shaderManager = 0;
    }
}

void QGL2PaintEngineExPrivate::updateTextureFilter(GLenum target, GLenum wrapMode, bool smoothPixmapTransform)
{
//    glActiveTexture(GL_TEXTURE0 + QT_BRUSH_TEXTURE_UNIT); //### Is it always this texture unit?

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
    uint alpha = qRound(c.alpha() * opacity);
    return QColor ( ((c.red() * alpha + 128) >> 8),
                    ((c.green() * alpha + 128) >> 8),
                    ((c.blue() * alpha + 128) >> 8),
                    alpha);
}


void QGL2PaintEngineExPrivate::setBrush(const QBrush* brush)
{
    currentBrush = brush;
    brushTextureDirty = true;
    brushUniformsDirty = true;
    shaderManager->setSrcPixelType(currentBrush->style());
    shaderManager->optimiseForBrushTransform(currentBrush->transform());
}


// Unless this gets used elsewhere, it's probably best to merge it into fillStencilWithVertexArray
void QGL2PaintEngineExPrivate::useSimpleShader()
{
    shaderManager->simpleProgram()->enable();

    if (matrixDirty)
        updateMatrix();

    if (simpleShaderMatrixUniformDirty) {
        shaderManager->simpleProgram()->setUniformValue("pmvMatrix", pmvMatrix);
        simpleShaderMatrixUniformDirty = false;
    }
}


Q_GLOBAL_STATIC(QGL2GradientCache, qt_opengl_gradient_cache)

void QGL2PaintEngineExPrivate::updateBrushTexture()
{
//     qDebug("QGL2PaintEngineExPrivate::updateBrushTexture()");
    Qt::BrushStyle style = currentBrush->style();

    if ( (style >= Qt::Dense1Pattern) && (style <= Qt::DiagCrossPattern) ) {
        // Get the image data for the pattern
        QImage texImage = qt_imageForBrush(style, true);

        glActiveTexture(GL_TEXTURE0 + QT_BRUSH_TEXTURE_UNIT);
        ctx->d_func()->bindTexture(texImage, GL_TEXTURE_2D, GL_RGBA, true);
        updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, true);
    }
    else if (style >= Qt::LinearGradientPattern && style <= Qt::ConicalGradientPattern) {
        // Gradiant brush: All the gradiants use the same texture

        const QGradient* g = currentBrush->gradient();

        // We apply global opacity in the fragment shaders, so we always pass 1.0
        // for opacity to the cache.
        GLuint texId = qt_opengl_gradient_cache()->getBuffer(*g, 1.0, ctx);

        if (g->spread() == QGradient::RepeatSpread || g->type() == QGradient::ConicalGradient)
            updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, true);
        else if (g->spread() == QGradient::ReflectSpread)
            updateTextureFilter(GL_TEXTURE_2D, GL_MIRRORED_REPEAT_IBM, true);
        else
            updateTextureFilter(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, true);

        glActiveTexture(GL_TEXTURE0 + QT_BRUSH_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, texId);
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
        shaderManager->currentProgram()->setUniformValue("fragmentColor", col);
    }
    else {
        // All other brushes have a transform and thus need the translation point:
        QPointF translationPoint;

        if (style <= Qt::DiagCrossPattern) {
            translationPoint = q->state()->brushOrigin;

            QColor col = premultiplyColor(currentBrush->color(), (GLfloat)q->state()->opacity);

            shaderManager->currentProgram()->setUniformValue("patternColor", col);

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue("halfViewportSize", halfViewportSize);
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

            shaderManager->currentProgram()->setUniformValue("linearData", linearData);

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue("halfViewportSize", halfViewportSize);
        }
        else if (style == Qt::ConicalGradientPattern) {
            const QConicalGradient *g = static_cast<const QConicalGradient *>(currentBrush->gradient());
            translationPoint   = g->center();

            GLfloat angle = -(g->angle() * 2 * Q_PI) / 360.0;

            shaderManager->currentProgram()->setUniformValue("angle", angle);

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue("halfViewportSize", halfViewportSize);
        }
        else if (style == Qt::RadialGradientPattern) {
            const QRadialGradient *g = static_cast<const QRadialGradient *>(currentBrush->gradient());
            QPointF realCenter = g->center();
            QPointF realFocal  = g->focalPoint();
            qreal   realRadius = g->radius();
            translationPoint   = realFocal;

            QPointF fmp = realCenter - realFocal;
            shaderManager->currentProgram()->setUniformValue("fmp", fmp);

            GLfloat fmp2_m_radius2 = -fmp.x() * fmp.x() - fmp.y() * fmp.y() + realRadius*realRadius;
            shaderManager->currentProgram()->setUniformValue("fmp2_m_radius2", fmp2_m_radius2);
            shaderManager->currentProgram()->setUniformValue("inverse_2_fmp2_m_radius2",
                                                             GLfloat(1.0 / (2.0*fmp2_m_radius2)));

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue("halfViewportSize", halfViewportSize);
        }
        else if (style == Qt::TexturePattern) {
            translationPoint = q->state()->brushOrigin;

            const QPixmap& texPixmap = currentBrush->texture();

            QSizeF invertedTextureSize( 1.0 / texPixmap.width(), 1.0 / texPixmap.height() );
            shaderManager->currentProgram()->setUniformValue("invertedTextureSize", invertedTextureSize);

            QVector2D halfViewportSize(width*0.5, height*0.5);
            shaderManager->currentProgram()->setUniformValue("halfViewportSize", halfViewportSize);
        }
        else
            qWarning("QGL2PaintEngineEx: Unimplemented fill style");

        QTransform translate(1, 0, 0, 1, -translationPoint.x(), -translationPoint.y());
        QTransform gl_to_qt(1, 0, 0, -1, 0, height);
        QTransform inv_matrix = gl_to_qt * (brushQTransform * q->state()->matrix).inverted() * translate;

        shaderManager->currentProgram()->setUniformValue("brushTransform", inv_matrix);
        shaderManager->currentProgram()->setUniformValue("brushTexture", QT_BRUSH_TEXTURE_UNIT);
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

void QGL2PaintEngineExPrivate::drawTexture(const QGLRect& dest, const QGLRect& src, const QSize &textureSize)
{
    transferMode(ImageDrawingMode);

    updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, q->state()->renderHints & QPainter::SmoothPixmapTransform);

    // Setup for texture drawing
    shaderManager->setSrcPixelType(QGLEngineShaderManager::ImageSrc);
    shaderManager->setTextureCoordsEnabled(true);
    prepareForDraw(false); // ###

    shaderManager->currentProgram()->setUniformValue("imageTexture", QT_IMAGE_TEXTURE_UNIT);

    GLfloat dx = 1.0 / textureSize.width();
    GLfloat dy = 1.0 / textureSize.height();

    QGLRect srcTextureRect(src.left*dx, 1.0 - src.top*dy, src.right*dx, 1.0 - src.bottom*dy);

    setCoords(staticVertexCoordinateArray, dest);
    setCoords(staticTextureCoordinateArray, srcTextureRect);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void QGL2PaintEngineExPrivate::transferMode(EngineMode newMode)
{
    if (newMode == mode)
        return;

    if (mode == TextDrawingMode || mode == ImageDrawingMode) {
        glDisableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);
        glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
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
        glEnable(GL_STENCIL_TEST);
        prepareForDraw(currentBrush->isOpaque());
        composite(vertexCoordinateArray.boundingRect());
        glDisable(GL_STENCIL_TEST);

        cleanStencilBuffer(vertexCoordinateArray.boundingRect());
    }
}


void QGL2PaintEngineExPrivate::fillStencilWithVertexArray(QGL2PEXVertexArray& vertexArray, bool useWindingFill)
{
//     qDebug("QGL2PaintEngineExPrivate::fillStencilWithVertexArray()");
    if (stencilBuferDirty) {
        // Clear the stencil buffer to zeros
        glDisable(GL_STENCIL_TEST);
        glStencilMask(0xFFFF); // Enable writing to stencil buffer, otherwise glClear wont do anything.
        glClearStencil(0); // Clear to zero
        glClear(GL_STENCIL_BUFFER_BIT);
        stencilBuferDirty = false;
    }

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writes
    glStencilMask(0xFFFF); // Enable stencil writes
    glStencilFunc(GL_ALWAYS, 0, 0xFFFF); // Always pass the stencil test

    // Setup the stencil op:
    if (useWindingFill) {
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP); // Inc. for front-facing triangle
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP); //Dec. for back-facing "holes"
    } else
        glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT); // Simply invert the stencil bit

    // No point in using a fancy gradiant shader for writing into the stencil buffer!
    useSimpleShader();

    glEnable(GL_STENCIL_TEST); // For some reason, this has to happen _after_ the simple shader is use()'d
    glDisable(GL_BLEND);

    // Draw the vertecies into the stencil buffer:
    drawVertexArrays(vertexArray, GL_TRIANGLE_FAN);

    // Enable color writes & disable stencil writes
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(0);
}

void QGL2PaintEngineExPrivate::cleanStencilBuffer(const QGLRect& area)
{
//     qDebug("QGL2PaintEngineExPrivate::cleanStencilBuffer()");
    useSimpleShader();

    GLfloat rectVerts[] = {
        area.left,  area.top,
        area.left,  area.bottom,
        area.right, area.bottom,
        area.right, area.top
    };

    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, rectVerts);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xFFFF); // Always pass the stencil test

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writes
    glStencilMask(0xFFFF); // Enable writing to stencil buffer
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO); // Write 0's to stencil buffer

    glDisable(GL_BLEND);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);

    // Enable color writes & disable stencil writes
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(0);
    glDisable(GL_STENCIL_TEST);
}

void QGL2PaintEngineExPrivate::prepareForDraw(bool srcPixelsAreOpaque)
{
    if (brushTextureDirty && mode != ImageDrawingMode)
        updateBrushTexture();

    if (compositionModeDirty)
        updateCompositionMode();

    if (matrixDirty)
        updateMatrix();

    const bool stateHasOpacity = q->state()->opacity < 0.99f;
    if ( (!srcPixelsAreOpaque || stateHasOpacity) &&
          q->state()->compositionMode() != QPainter::CompositionMode_Source)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    bool useGlobalOpacityUniform = stateHasOpacity;
    if (stateHasOpacity && (mode != ImageDrawingMode)) {
        // Using a brush
        bool brushIsPattern = (currentBrush->style() >= Qt::Dense1Pattern) &&
                              (currentBrush->style() <= Qt::DiagCrossPattern);

        if ((currentBrush->style() == Qt::SolidPattern) || brushIsPattern)
            useGlobalOpacityUniform = false; // Global opacity handled by srcPixel shader
    }
    shaderManager->setUseGlobalOpacity(useGlobalOpacityUniform);


    // If the shader program needs changing, we change it and mark all uniforms as dirty
    if (shaderManager->useCorrectShaderProg()) {
        // The shader program has changed so mark all uniforms as dirty:
        brushUniformsDirty = true;
        shaderMatrixUniformDirty = true;
    }

    if (brushUniformsDirty && mode != ImageDrawingMode)
        updateBrushUniforms();

    if (shaderMatrixUniformDirty) {
        shaderManager->currentProgram()->setUniformValue("pmvMatrix", pmvMatrix);
        shaderMatrixUniformDirty = false;
    }

    if (useGlobalOpacityUniform)
        shaderManager->currentProgram()->setUniformValue("globalOpacity", (GLfloat)q->state()->opacity);
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
}

void QGL2PaintEngineEx::compositionModeChanged()
{
//     qDebug("QGL2PaintEngineEx::compositionModeChanged()");
    Q_D(QGL2PaintEngineEx);
    d->compositionModeDirty = true;
}

void QGL2PaintEngineEx::renderHintsChanged()
{
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
    ctx->d_func()->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, true);

    //FIXME: we should use hasAlpha() instead, but that's SLOW at the moment
    if ((state()->opacity < 0.99f) || pixmap.hasAlphaChannel())
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    d->drawTexture(dest, src, pixmap.size());
}

void QGL2PaintEngineEx::drawImage(const QRectF& dest, const QImage& image, const QRectF& src,
                        Qt::ImageConversionFlags)
{
    Q_D(QGL2PaintEngineEx);
    ensureActive();
    d->transferMode(ImageDrawingMode);

    QGLContext *ctx = d->ctx;
    glActiveTexture(GL_TEXTURE0 + QT_IMAGE_TEXTURE_UNIT);
    ctx->d_func()->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, true);

    if ((state()->opacity < 0.99f) || image.hasAlphaChannel())
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    d->drawTexture(dest, src, image.size());
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
    transferMode(TextDrawingMode);

    Q_Q(QGL2PaintEngineEx);
    QOpenGL2PaintEngineState *s = q->state();

    QVarLengthArray<QFixedPoint> positions;
    QVarLengthArray<glyph_t> glyphs;
    QTransform matrix;
    matrix.translate(p.x(), p.y());
    ti.fontEngine->getGlyphPositions(ti.glyphs, matrix, ti.flags, glyphs, positions);


    QFontEngineGlyphCache::Type glyphType = ti.fontEngine->glyphFormat >= 0
        ? QFontEngineGlyphCache::Type(ti.fontEngine->glyphFormat)
        : QFontEngineGlyphCache::Raster_A8;

    GLenum maskFormat = GL_RGBA;
    if (glyphType == QFontEngineGlyphCache::Raster_A8) {
        shaderManager->setMaskType(QGLEngineShaderManager::PixelMask);
//        maskFormat = GL_ALPHA;
    }
    else if (glyphType == QFontEngineGlyphCache::Raster_RGBMask)
        shaderManager->setMaskType(QGLEngineShaderManager::SubPixelMask);
    //### TODO: Gamma correction
    shaderManager->setTextureCoordsEnabled(true);


    QImageTextureGlyphCache *cache =
        (QImageTextureGlyphCache *) ti.fontEngine->glyphCache(glyphType, s->matrix);
    if (!cache) {
        cache = new QImageTextureGlyphCache(glyphType, s->matrix);
        ti.fontEngine->setGlyphCache(glyphType, cache);
    }

    cache->populate(ti, glyphs, positions);

    const QImage &image = cache->image();
    int margin = cache->glyphMargin();

    if (image.isNull())
        return;

    GLfloat dx = 1.0 / image.width();
    GLfloat dy = 1.0 / image.height();

    QGLPoint *oldVertexCoordinateDataPtr = vertexCoordinateArray.data();
    QGLPoint *oldTextureCoordinateDataPtr = textureCoordinateArray.data();

    vertexCoordinateArray.clear();
    textureCoordinateArray.clear();

    for (int i=0; i<glyphs.size(); ++i) {
        const QTextureGlyphCache::Coord &c = cache->coords.value(glyphs[i]);
        int x = positions[i].x.toInt() + c.baseLineX - margin;
        int y = positions[i].y.toInt() - c.baseLineY - margin;

        vertexCoordinateArray.addRect(QRectF(x, y, c.w, c.h));
        textureCoordinateArray.addRect(QRectF(c.x*dx, 1 - c.y*dy, c.w * dx, -c.h * dy));
    }

    glActiveTexture(GL_TEXTURE0 + QT_MASK_TEXTURE_UNIT);
    ctx->d_func()->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, true);
    updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, false);

    QBrush pensBrush = q->state()->pen.brush();
    setBrush(&pensBrush);

    prepareForDraw(false); // Text always causes src pixels to be transparent

    shaderManager->currentProgram()->setUniformValue("maskTexture", QT_MASK_TEXTURE_UNIT);

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

    d->drawable.makeCurrent();
    QSize sz = d->drawable.size();
    d->width = sz.width();
    d->height = sz.height();
    d->mode = BrushDrawingMode;

#if !defined(QT_OPENGL_ES_2)
    qt_resolve_version_2_0_functions(d->ctx);
#endif

    if (!d->shaderManager)
        d->shaderManager = new QGLEngineShaderManager(d->ctx);

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
    d->stencilBuferDirty = true;

    d->use_system_clip = !systemClip().isEmpty();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

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
        d->transferMode(ImageDrawingMode);

        source->bind(false);

        glDisable(GL_BLEND);

        QRect rect(0, 0, source->width(), source->height());
        d->drawTexture(QRectF(rect), QRectF(rect), rect.size());
    }

    updateClipRegion(QRegion(), Qt::NoClip);
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

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);

        glViewport(0, 0, d->width, d->height);
        setState(state());
        d->updateDepthClip();
    }
}


/////////////////////////////////// State/Clipping stolen from QOpenGLPaintEngine //////////////////////////////////////////

void QGL2PaintEngineEx::clipEnabledChanged()
{
    Q_D(QGL2PaintEngineEx);

    d->updateDepthClip();
}

void QGL2PaintEngineEx::clip(const QVectorPath &path, Qt::ClipOperation op)
{
//     qDebug("QGL2PaintEngineEx::clip()");
    const qreal *points = path.points();
    const QPainterPath::ElementType *types = path.elements();
    if (!types && path.shape() == QVectorPath::RectangleHint) {
        QRectF r(points[0], points[1], points[4]-points[0], points[5]-points[1]);
        updateClipRegion(QRegion(r.toRect()), op);
        return;
    }

    QPainterPath p;
    if (types) {
        int id = 0;
        for (int i=0; i<path.elementCount(); ++i) {
            switch(types[i]) {
            case QPainterPath::MoveToElement:
                p.moveTo(QPointF(points[id], points[id+1]));
                id+=2;
                break;
            case QPainterPath::LineToElement:
                p.lineTo(QPointF(points[id], points[id+1]));
                id+=2;
                break;
            case QPainterPath::CurveToElement: {
                QPointF p1(points[id], points[id+1]);
                QPointF p2(points[id+2], points[id+3]);
                QPointF p3(points[id+4], points[id+5]);
                p.cubicTo(p1, p2, p3);
                id+=6;
                break;
            }
            case QPainterPath::CurveToDataElement:
                ;
                break;
            }
        }
    } else if (!path.isEmpty()) {
        p.moveTo(QPointF(points[0], points[1]));
        int id = 2;
        for (int i=1; i<path.elementCount(); ++i) {
            p.lineTo(QPointF(points[id], points[id+1]));
            id+=2;
        }
    }
    if (path.hints() & QVectorPath::WindingFill)
        p.setFillRule(Qt::WindingFill);

    updateClipRegion(QRegion(p.toFillPolygon().toPolygon(), p.fillRule()), op);
    return;
}

void QGL2PaintEngineEx::updateClipRegion(const QRegion &clipRegion, Qt::ClipOperation op)
{
//     qDebug("QGL2PaintEngineEx::updateClipRegion()");
    Q_D(QGL2PaintEngineEx);

    QRegion sysClip = systemClip();
    if (op == Qt::NoClip && !d->use_system_clip) {
        state()->hasClipping = false;
        state()->clipRegion = QRegion();
        d->updateDepthClip();
        return;
    }

    bool isScreenClip = false;
    if (!d->use_system_clip) {
        QVector<QRect> untransformedRects = clipRegion.rects();

        if (untransformedRects.size() == 1) {
            QPainterPath path;
            path.addRect(untransformedRects[0]);
            //path = d->matrix.map(path);
            path = state()->matrix.map(path);

//             if (path.contains(QRectF(QPointF(), d->drawable.size())))
//                 isScreenClip = true;
            if (path.contains(QRectF(0.0, 0.0, d->width, d->height)))
                isScreenClip = true;
        }
    }

//     QRegion region = isScreenClip ? QRegion() : clipRegion * d->matrix;
    QRegion region = isScreenClip ? QRegion() : clipRegion * state()->matrix;
    switch (op) {
    case Qt::NoClip:
        if (!d->use_system_clip)
            break;
        state()->clipRegion = sysClip;
        break;
    case Qt::IntersectClip:
        if (isScreenClip)
            return;
        if (state()->hasClipping) {
            state()->clipRegion &= region;
            break;
        }
        // fall through
    case Qt::ReplaceClip:
        if (d->use_system_clip && !sysClip.isEmpty())
            state()->clipRegion = region & sysClip;
        else
            state()->clipRegion = region;
        break;
    case Qt::UniteClip:
        state()->clipRegion |= region;
        if (d->use_system_clip && !sysClip.isEmpty())
            state()->clipRegion &= sysClip;
        break;
    default:
        break;
    }

    if (isScreenClip) {
        state()->hasClipping = false;
        state()->clipRegion = QRegion();
    } else {
        state()->hasClipping = op != Qt::NoClip || d->use_system_clip;
    }

    d->updateDepthClip();
}

void QGL2PaintEngineExPrivate::systemStateChanged()
{
    Q_Q(QGL2PaintEngineEx);
    use_system_clip = !systemClip.isEmpty();

    if (q->painter()->hasClipping())
        q->updateClipRegion(q->painter()->clipRegion(), Qt::ReplaceClip);
    else
        q->updateClipRegion(QRegion(), Qt::NoClip);
}

void QGL2PaintEngineExPrivate::updateDepthClip()
{
//     qDebug("QGL2PaintEngineExPrivate::updateDepthClip()");

    Q_Q(QGL2PaintEngineEx);

    q->ensureActive();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

    if (!q->state()->hasClipping)
        return;

    const QVector<QRect> rects = q->state()->clipEnabled ? q->state()->clipRegion.rects() : q->systemClip().rects();
    if (rects.size() == 1) {
        QRect fastClip = rects.at(0);

        glEnable(GL_SCISSOR_TEST);

        const int left = fastClip.left();
        const int width = fastClip.width();
        const int bottom = height - (fastClip.bottom() + 1);
        const int height = fastClip.height();

        glScissor(left, bottom, width, height);
        return;
    }

    glClearDepth(0x0);
    glDepthMask(true);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearDepth(0x1);

    glEnable(GL_SCISSOR_TEST);
    for (int i = 0; i < rects.size(); ++i) {
        QRect rect = rects.at(i);

        const int left = rect.left();
        const int width = rect.width();
        const int bottom = height - (rect.bottom() + 1);
        const int height = rect.height();

        glScissor(left, bottom, width, height);

        glClear(GL_DEPTH_BUFFER_BIT);
    }
    glDisable(GL_SCISSOR_TEST);

    glDepthMask(false);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
}



void QGL2PaintEngineEx::setState(QPainterState *new_state)
{
//     qDebug("QGL2PaintEngineEx::setState()");

    Q_D(QGL2PaintEngineEx);

    QOpenGL2PaintEngineState *s = static_cast<QOpenGL2PaintEngineState *>(new_state);

    QOpenGL2PaintEngineState *old_state = state();
    const bool needsDepthClipUpdate = !old_state
            || s->clipEnabled != old_state->clipEnabled
            || (s->clipEnabled && s->clipRegion != old_state->clipRegion);

    QPaintEngineEx::setState(s);

    if (needsDepthClipUpdate)
        d->updateDepthClip();

    d->matrixDirty = true;
    d->compositionModeDirty = true;
    d->brushTextureDirty = true;
    d->brushUniformsDirty = true;
    d->simpleShaderMatrixUniformDirty = true;
    d->shaderMatrixUniformDirty = true;
}

QPainterState *QGL2PaintEngineEx::createState(QPainterState *orig) const
{
    QOpenGL2PaintEngineState *s;
    if (!orig)
        s = new QOpenGL2PaintEngineState();
    else
        s = new QOpenGL2PaintEngineState(*static_cast<QOpenGL2PaintEngineState *>(orig));

    return s;
}

QOpenGL2PaintEngineState::QOpenGL2PaintEngineState(QOpenGL2PaintEngineState &other)
    : QPainterState(other)
{
    clipRegion = other.clipRegion;
    hasClipping = other.hasClipping;
}

QOpenGL2PaintEngineState::QOpenGL2PaintEngineState()
{
    hasClipping = false;
}

QOpenGL2PaintEngineState::~QOpenGL2PaintEngineState()
{
}

QT_END_NAMESPACE
