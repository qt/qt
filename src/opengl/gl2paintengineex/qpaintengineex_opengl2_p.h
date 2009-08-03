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

enum EngineMode {
    ImageDrawingMode,
    TextDrawingMode,
    BrushDrawingMode
};

QT_BEGIN_NAMESPACE

class QGL2PaintEngineExPrivate;


class QOpenGL2PaintEngineState : public QPainterState
{
public:
    QOpenGL2PaintEngineState(QOpenGL2PaintEngineState &other);
    QOpenGL2PaintEngineState();
    ~QOpenGL2PaintEngineState();

    bool needsDepthBufferClear;
    qreal depthBufferClearValue;

    bool depthTestEnabled;
    bool scissorTestEnabled;
    qreal currentDepth;
    qreal maxDepth;

    bool canRestoreClip;
};


class QGL2PaintEngineEx : public QPaintEngineEx
{
    Q_DECLARE_SCOPED_PRIVATE(QGL2PaintEngineEx)
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
    virtual void drawTextItem(const QPointF &p, const QTextItem &textItem);

    Type type() const { return OpenGL; }

    void setState(QPainterState *s);
    QPainterState *createState(QPainterState *orig) const;
    inline QOpenGL2PaintEngineState *state() {
        return static_cast<QOpenGL2PaintEngineState *>(QPaintEngineEx::state());
    }
    inline const QOpenGL2PaintEngineState *state() const {
        return static_cast<const QOpenGL2PaintEngineState *>(QPaintEngineEx::state());
    }
    virtual void sync();

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
            currentBrush( &(q->state()->brush) ),
            inverseScale(1),
            shaderManager(0)
    { }

    ~QGL2PaintEngineExPrivate();

    void updateBrushTexture();
    void updateBrushUniforms();
    void updateMatrix();
    void updateCompositionMode();
    void updateTextureFilter(GLenum target, GLenum wrapMode, bool smoothPixmapTransform, GLuint id = -1);

    void setBrush(const QBrush* brush);

    void transferMode(EngineMode newMode);

    // fill, drawOutline, drawTexture & drawCachedGlyphs are the rendering entry points:
    void fill(const QVectorPath &path);
    void drawOutline(const QVectorPath& path);
    void drawTexture(const QGLRect& dest, const QGLRect& src, const QSize &textureSize, bool opaque, bool pattern = false);
    void drawCachedGlyphs(const QPointF &p, const QTextItemInt &ti);

    void drawVertexArrays(QGL2PEXVertexArray& vertexArray, GLenum primitive);
        // ^ draws whatever is in the vertex array
    void composite(const QGLRect& boundingRect);
        // ^ Composites the bounding rect onto dest buffer
    void fillStencilWithVertexArray(QGL2PEXVertexArray& vertexArray, bool useWindingFill);
        // ^ Calls drawVertexArrays to render into stencil buffer

    bool prepareForDraw(bool srcPixelsAreOpaque);
        // ^ returns whether the current program changed or not

    inline void useSimpleShader();
    inline QColor premultiplyColor(QColor c, GLfloat opacity);

    QGL2PaintEngineEx* q;
    QGLDrawable drawable;
    int width, height;
    QGLContext *ctx;
    EngineMode mode;

    mutable QOpenGL2PaintEngineState *last_created_state;

    // Dirty flags
    bool matrixDirty; // Implies matrix uniforms are also dirty
    bool compositionModeDirty;
    bool brushTextureDirty;
    bool brushUniformsDirty;
    bool simpleShaderMatrixUniformDirty;
    bool shaderMatrixUniformDirty;
    bool stencilBufferDirty;
    bool depthUniformDirty;
    bool simpleShaderDepthUniformDirty;
    bool opacityUniformDirty;

    const QBrush*    currentBrush; // May not be the state's brush!

    GLfloat     inverseScale;

    QGL2PEXVertexArray vertexCoordinateArray;
    QGL2PEXVertexArray textureCoordinateArray;

    GLfloat staticVertexCoordinateArray[8];
    GLfloat staticTextureCoordinateArray[8];

    GLfloat pmvMatrix[4][4];

    QGLEngineShaderManager* shaderManager;

    void writeClip(const QVectorPath &path, float depth);
    void updateDepthScissorTest();
    void regenerateDepthClip();
    void systemStateChanged();
    uint use_system_clip : 1;

    enum Uniform {
        ImageTexture,
        PatternColor,
        GlobalOpacity,
        Depth,
        PmvMatrix,
        MaskTexture,
        FragmentColor,
        LinearData,
        Angle,
        HalfViewportSize,
        Fmp,
        Fmp2MRadius2,
        Inverse2Fmp2MRadius2,
        InvertedTextureSize,
        BrushTransform,
        BrushTexture,
        NumUniforms
    };

    uint location(Uniform uniform)
    {
        return shaderManager->getUniformLocation(uniformIdentifiers[uniform]);
    }

    uint uniformIdentifiers[NumUniforms];
    GLuint lastTexture;

    bool needsSync;
};

QT_END_NAMESPACE

#endif
