/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QGRAPHICSCONTEXT_OPENGL2_P_H
#define QGRAPHICSCONTEXT_OPENGL2_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QDebug>

#include <private/qpaintengineex_p.h>
#include <private/qglengineshadermanager_p.h>
#include <private/qgl2pexvertexarray_p.h>
#include <private/qglpaintdevice_p.h>
#include <private/qglpixmapfilter_p.h>
#include <private/qfontengine_p.h>
#include <private/qdatabuffer_p.h>

enum EngineMode {
    ImageDrawingMode,
    TextDrawingMode,
    BrushDrawingMode,
    ImageArrayDrawingMode
};

QT_BEGIN_NAMESPACE

class QGL2PaintEngineExPrivate;


class QOpenGL2PaintEngineState : public QPainterState
{
public:
    QOpenGL2PaintEngineState(QOpenGL2PaintEngineState &other);
    QOpenGL2PaintEngineState();
    ~QOpenGL2PaintEngineState();

    bool needsClipBufferClear;

    bool clipTestEnabled;
    bool scissorTestEnabled;
    uint currentClip;

    bool canRestoreClip;
    QRect rectangleClip;
};

class Q_OPENGL_EXPORT QGL2PaintEngineEx : public QPaintEngineEx
{
    Q_DECLARE_PRIVATE(QGL2PaintEngineEx)
public:
    QGL2PaintEngineEx();
    ~QGL2PaintEngineEx();

    bool begin(QPaintDevice *device);
    bool end();

    void ensureActive();

    virtual void fill(const QVectorPath &path, const QBrush &brush);
    virtual void stroke(const QVectorPath &path, const QPen &pen);
    virtual void clip(const QVectorPath &path, Qt::ClipOperation op);

    virtual void clipEnabledChanged();
    virtual void penChanged();
    virtual void brushChanged();
    virtual void brushOriginChanged();
    virtual void opacityChanged();
    virtual void compositionModeChanged();
    virtual void renderHintsChanged();
    virtual void transformChanged();


    virtual void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);
    virtual void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                           Qt::ImageConversionFlags flags = Qt::AutoColor);
    virtual void drawTexture(const QRectF &r, GLuint textureId, const QSize &size, const QRectF &sr);

    virtual void drawTextItem(const QPointF &p, const QTextItem &textItem);

    virtual void drawPixmaps(const QDrawPixmaps::Data *drawingData, int dataCount, const QPixmap &pixmap, QDrawPixmaps::DrawingHints hints);

    Type type() const { return OpenGL2; }

    void setState(QPainterState *s);
    QPainterState *createState(QPainterState *orig) const;
    inline QOpenGL2PaintEngineState *state() {
        return static_cast<QOpenGL2PaintEngineState *>(QPaintEngineEx::state());
    }
    inline const QOpenGL2PaintEngineState *state() const {
        return static_cast<const QOpenGL2PaintEngineState *>(QPaintEngineEx::state());
    }

    void beginNativePainting();
    void endNativePainting();

    const QGLContext* context();

    QPixmapFilter *pixmapFilter(int type, const QPixmapFilter *prototype);

    void setRenderTextActive(bool);

private:
    Q_DISABLE_COPY(QGL2PaintEngineEx)
};


class QGL2PaintEngineExPrivate : public QPaintEngineExPrivate
{
    Q_DECLARE_PUBLIC(QGL2PaintEngineEx)
public:
    QGL2PaintEngineExPrivate(QGL2PaintEngineEx *q_ptr) :
            q(q_ptr),
            width(0), height(0),
            ctx(0),
            currentBrush(0),
            inverseScale(1),
            shaderManager(0),
            inRenderText(false)
    { }

    ~QGL2PaintEngineExPrivate();

    void updateBrushTexture();
    void updateBrushUniforms();
    void updateMatrix();
    void updateCompositionMode();
    void updateTextureFilter(GLenum target, GLenum wrapMode, bool smoothPixmapTransform, GLuint id = -1);

    void setBrush(const QBrush* brush);

    void transferMode(EngineMode newMode);
    void resetGLState();

    // fill, drawOutline, drawTexture & drawCachedGlyphs are the rendering entry points:
    void fill(const QVectorPath &path);
    void drawOutline(const QVectorPath& path);
    void drawTexture(const QGLRect& dest, const QGLRect& src, const QSize &textureSize, bool opaque, bool pattern = false);
    void drawCachedGlyphs(const QPointF &p, QFontEngineGlyphCache::Type glyphType, const QTextItemInt &ti);

    void drawVertexArrays(QGL2PEXVertexArray& vertexArray, GLenum primitive);
        // ^ draws whatever is in the vertex array
    void composite(const QGLRect& boundingRect);
        // ^ Composites the bounding rect onto dest buffer
    void fillStencilWithVertexArray(QGL2PEXVertexArray& vertexArray, bool useWindingFill);
        // ^ Calls drawVertexArrays to render into stencil buffer

    bool prepareForDraw(bool srcPixelsAreOpaque);
        // ^ returns whether the current program changed or not

    inline void useSimpleShader();

    void prepareDepthRangeForRenderText();
    void restoreDepthRangeForRenderText();

    static QGLEngineShaderManager* shaderManagerForEngine(QGL2PaintEngineEx *engine) { return engine->d_func()->shaderManager; }

    QGL2PaintEngineEx* q;
    QGLPaintDevice* device;
    int width, height;
    QGLContext *ctx;
    EngineMode mode;
    QFontEngineGlyphCache::Type glyphCacheType;

    mutable QOpenGL2PaintEngineState *last_created_state;

    // Dirty flags
    bool matrixDirty; // Implies matrix uniforms are also dirty
    bool compositionModeDirty;
    bool brushTextureDirty;
    bool brushUniformsDirty;
    bool simpleShaderMatrixUniformDirty;
    bool shaderMatrixUniformDirty;
    bool opacityUniformDirty;

    QRegion dirtyStencilRegion;
    QRect currentScissorBounds;
    uint maxClip;

    const QBrush*    currentBrush; // May not be the state's brush!

    GLfloat     inverseScale;

    QGL2PEXVertexArray vertexCoordinateArray;
    QGL2PEXVertexArray textureCoordinateArray;
    QDataBuffer<GLfloat> opacityArray;

    GLfloat staticVertexCoordinateArray[8];
    GLfloat staticTextureCoordinateArray[8];

    GLfloat pmvMatrix[4][4];

    QGLEngineShaderManager* shaderManager;

    void clearClip(uint value);
    void writeClip(const QVectorPath &path, uint value);
    void resetClipIfNeeded();

    void updateClipScissorTest();
    void setScissor(const QRect &rect);
    void regenerateClip();
    void systemStateChanged();
    uint use_system_clip : 1;

    uint location(QGLEngineShaderManager::Uniform uniform)
    {
        return shaderManager->getUniformLocation(uniform);
    }

    GLuint lastTexture;

    bool needsSync;
    bool inRenderText;

    GLfloat depthRange[2];

    float textureInvertedY;

    QScopedPointer<QPixmapFilter> convolutionFilter;
    QScopedPointer<QPixmapFilter> colorizeFilter;
    QScopedPointer<QPixmapFilter> blurFilter;
    QScopedPointer<QPixmapFilter> fastBlurFilter;
    QScopedPointer<QPixmapFilter> dropShadowFilter;
    QScopedPointer<QPixmapFilter> fastDropShadowFilter;
};

QT_END_NAMESPACE

#endif
