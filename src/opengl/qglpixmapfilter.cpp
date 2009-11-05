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

#include "private/qpixmapfilter_p.h"
#include "private/qpixmapdata_gl_p.h"
#include "private/qpaintengineex_opengl2_p.h"
#include "private/qglengineshadermanager_p.h"
#include "qglpixmapfilter_p.h"
#include "qgraphicssystem_gl_p.h"
#include "qpaintengine_opengl_p.h"
#include "qcache.h"

#include "qglframebufferobject.h"
#include "qglshaderprogram.h"
#include "qgl_p.h"

#include "private/qapplication_p.h"
#include "private/qmath_p.h"


QT_BEGIN_NAMESPACE

void QGLPixmapFilterBase::bindTexture(const QPixmap &src) const
{
    const_cast<QGLContext *>(QGLContext::currentContext())->d_func()->bindTexture(src, GL_TEXTURE_2D, GL_RGBA, QGLContext::BindOptions(QGLContext::DefaultBindOption | QGLContext::MemoryManagedBindOption));
}

void QGLPixmapFilterBase::drawImpl(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF& source) const
{
    processGL(painter, pos, src, source);
}

class QGLPixmapColorizeFilter: public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapColorizeFilter>
{
public:
    QGLPixmapColorizeFilter();

    void setUniforms(QGLShaderProgram *program);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &pixmap, const QRectF &srcRect) const;
};

class QGLPixmapConvolutionFilter: public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapConvolutionFilter>
{
public:
    QGLPixmapConvolutionFilter();
    ~QGLPixmapConvolutionFilter();

    void setUniforms(QGLShaderProgram *program);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:
    QByteArray generateConvolutionShader() const;

    mutable QSize m_srcSize;
    mutable int m_prevKernelSize;
};

class QGLPixmapBlurFilter : public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapBlurFilter>
{
public:
    QGLPixmapBlurFilter(Qt::RenderHint hint);

    void setUniforms(QGLShaderProgram *program);

    static QByteArray generateGaussianShader(int radius, bool singlePass = false, bool dropShadow = false);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:

    mutable QSize m_textureSize;
    mutable bool m_horizontalBlur;
    mutable bool m_singlePass;

    mutable bool m_haveCached;
    mutable int m_cachedRadius;
    mutable Qt::RenderHint m_hint;
};

class QGLPixmapDropShadowFilter : public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapDropShadowFilter>
{
public:
    QGLPixmapDropShadowFilter(Qt::RenderHint hint);

    void setUniforms(QGLShaderProgram *program);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:
    mutable QSize m_textureSize;
    mutable bool m_horizontalBlur;
    mutable bool m_singlePass;

    mutable bool m_haveCached;
    mutable int m_cachedRadius;
    mutable Qt::RenderHint m_hint;
};

extern QGLWidget *qt_gl_share_widget();

QPixmapFilter *QGL2PaintEngineEx::pixmapFilter(int type, const QPixmapFilter *prototype)
{
    Q_D(QGL2PaintEngineEx);
    switch (type) {
    case QPixmapFilter::ColorizeFilter:
        if (!d->colorizeFilter)
            d->colorizeFilter.reset(new QGLPixmapColorizeFilter);
        return d->colorizeFilter.data();

    case QPixmapFilter::BlurFilter: {
        const QPixmapBlurFilter *proto = static_cast<const QPixmapBlurFilter *>(prototype);
        if (proto->blurHint() == Qt::PerformanceHint || proto->radius() <= 5) {
            if (!d->fastBlurFilter)
                d->fastBlurFilter.reset(new QGLPixmapBlurFilter(Qt::PerformanceHint));
            return d->fastBlurFilter.data();
        }
        if (!d->blurFilter)
            d->blurFilter.reset(new QGLPixmapBlurFilter(Qt::QualityHint));
        return d->blurFilter.data();
        }

    case QPixmapFilter::DropShadowFilter: {
        const QPixmapDropShadowFilter *proto = static_cast<const QPixmapDropShadowFilter *>(prototype);
        if (proto->blurRadius() <= 5) {
            if (!d->fastDropShadowFilter)
                d->fastDropShadowFilter.reset(new QGLPixmapDropShadowFilter(Qt::PerformanceHint));
            return d->fastDropShadowFilter.data();
        }
        if (!d->dropShadowFilter)
            d->dropShadowFilter.reset(new QGLPixmapDropShadowFilter(Qt::QualityHint));
        return d->dropShadowFilter.data();
        }

    case QPixmapFilter::ConvolutionFilter:
        if (!d->convolutionFilter)
            d->convolutionFilter.reset(new QGLPixmapConvolutionFilter);
        return d->convolutionFilter.data();

    default: break;
    }
    return QPaintEngineEx::pixmapFilter(type, prototype);
}

static const char *qt_gl_colorize_filter =
        "uniform lowp vec4 colorizeColor;"
        "uniform lowp float colorizeStrength;"
        "lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords)"
        "{"
        "        lowp vec4 srcPixel = texture2D(src, srcCoords);"
        "        lowp float gray = dot(srcPixel.rgb, vec3(0.212671, 0.715160, 0.072169));"
        "        lowp vec3 colorized = 1.0-((1.0-gray)*(1.0-colorizeColor.rgb));"
        "        return vec4(mix(srcPixel.rgb, colorized * srcPixel.a, colorizeStrength), srcPixel.a);"
        "}";

QGLPixmapColorizeFilter::QGLPixmapColorizeFilter()
{
    setSource(qt_gl_colorize_filter);
}

bool QGLPixmapColorizeFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &) const
{
    QGLPixmapColorizeFilter *filter = const_cast<QGLPixmapColorizeFilter *>(this);

    filter->setOnPainter(painter);
    painter->drawPixmap(pos, src);
    filter->removeFromPainter(painter);

    return true;
}

void QGLPixmapColorizeFilter::setUniforms(QGLShaderProgram *program)
{
    program->setUniformValue("colorizeColor", color());
    program->setUniformValue("colorizeStrength", float(strength()));
}

void QGLPixmapConvolutionFilter::setUniforms(QGLShaderProgram *program)
{
    const qreal *kernel = convolutionKernel();
    int kernelWidth = columns();
    int kernelHeight = rows();
    int kernelSize = kernelWidth * kernelHeight;

    QVarLengthArray<GLfloat> matrix(kernelSize);
    QVarLengthArray<GLfloat> offset(kernelSize * 2);

    for(int i = 0; i < kernelSize; ++i)
        matrix[i] = kernel[i];

    for(int y = 0; y < kernelHeight; ++y) {
        for(int x = 0; x < kernelWidth; ++x) {
            offset[(y * kernelWidth + x) * 2] = x - (kernelWidth / 2);
            offset[(y * kernelWidth + x) * 2 + 1] = (kernelHeight / 2) - y;
        }
    }

    const qreal iw = 1.0 / m_srcSize.width();
    const qreal ih = 1.0 / m_srcSize.height();
    program->setUniformValue("inv_texture_size", iw, ih);
    program->setUniformValueArray("matrix", matrix.constData(), kernelSize, 1);
    program->setUniformValueArray("offset", offset.constData(), kernelSize, 2);
}

// generates convolution filter code for arbitrary sized kernel
QByteArray QGLPixmapConvolutionFilter::generateConvolutionShader() const {
    QByteArray code;
    int kernelWidth = columns();
    int kernelHeight = rows();
    int kernelSize = kernelWidth * kernelHeight;
    code.append("uniform highp vec2 inv_texture_size;\n"
                "uniform mediump float matrix[");
    code.append(QByteArray::number(kernelSize));
    code.append("];\n"
                "uniform highp vec2 offset[");
    code.append(QByteArray::number(kernelSize));
    code.append("];\n");
    code.append("lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords) {\n");

    code.append("  int i = 0;\n"
                "  lowp vec4 sum = vec4(0.0);\n"
                "  for (i = 0; i < ");
    code.append(QByteArray::number(kernelSize));
    code.append("; i++) {\n"
                "    sum += matrix[i] * texture2D(src,srcCoords+inv_texture_size*offset[i]);\n"
                "  }\n"
                "  return sum;\n"
                "}");
    return code;
}

QGLPixmapConvolutionFilter::QGLPixmapConvolutionFilter()
    : m_prevKernelSize(-1)
{
}

QGLPixmapConvolutionFilter::~QGLPixmapConvolutionFilter()
{
}

bool QGLPixmapConvolutionFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    QGLPixmapConvolutionFilter *filter = const_cast<QGLPixmapConvolutionFilter *>(this);

    m_srcSize = src.size();

    int kernelSize = rows() * columns();
    if (m_prevKernelSize == -1 || m_prevKernelSize != kernelSize) {
        filter->setSource(generateConvolutionShader());
        m_prevKernelSize = kernelSize;
    }

    filter->setOnPainter(painter);
    painter->drawPixmap(pos, src, srcRect);
    filter->removeFromPainter(painter);

    return true;
}

static const char *qt_gl_texture_sampling_helper =
    "lowp float texture2DAlpha(lowp sampler2D src, highp vec2 srcCoords) {\n"
    "   return texture2D(src, srcCoords).a;\n"
    "}\n";

static const char *qt_gl_clamped_texture_sampling_helper =
    "highp vec4 texture_dimensions;\n" // x = width, y = height, z = 0.5/width, w = 0.5/height
    "lowp float clampedTexture2DAlpha(lowp sampler2D src, highp vec2 srcCoords) {\n"
    "   highp vec2 clampedCoords = clamp(srcCoords, texture_dimensions.zw, vec2(1.0) - texture_dimensions.zw);\n"
    "   highp vec2 t = clamp(min(srcCoords, vec2(1.0) - srcCoords) * srcDim + 0.5, 0.0, 1.0);\n"
    "   return texture2D(src, clampedCoords).a * t.x * t.y;\n"
    "}\n"
    "lowp vec4 clampedTexture2D(lowp sampler2D src, highp vec2 srcCoords) {\n"
    "   highp vec2 clampedCoords = clamp(srcCoords, texture_dimensions.zw, vec2(1.0) - texture_dimensions.zw);\n"
    "   highp vec2 t = clamp(min(srcCoords, vec2(1.0) - srcCoords) * srcDim + 0.5, 0.0, 1.0);\n"
    "   return texture2D(src, clampedCoords) * t.x * t.y;\n"
    "}\n";

static QByteArray qt_gl_convertToClamped(const QByteArray &source)
{
    QByteArray result;
    result.append(qt_gl_clamped_texture_sampling_helper);
    result.append(QByteArray(source).replace("texture2DAlpha", "clampedTexture2DAlpha")
                                    .replace("texture2D", "clampedTexture2D"));
    return result;
}

QGLPixmapBlurFilter::QGLPixmapBlurFilter(Qt::RenderHint hint)
    : m_haveCached(false)
    , m_cachedRadius(0)
    , m_hint(hint)
{
}

bool QGLPixmapBlurFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &) const
{
    QGLPixmapBlurFilter *filter = const_cast<QGLPixmapBlurFilter *>(this);

    int actualRadius = qRound(radius());
    int filterRadius = actualRadius;
    int fastRadii[] = { 1, 2, 3, 5, 8, 15, 25 };
    if (m_hint == Qt::PerformanceHint) {
        uint i = 0;
        for (; i < (sizeof(fastRadii)/sizeof(*fastRadii))-1; ++i) {
            if (fastRadii[i+1] > filterRadius)
                break;
        }
        filterRadius = fastRadii[i];
    }

    m_singlePass = filterRadius <= 3;

    if (!m_haveCached || filterRadius != m_cachedRadius) {
        // Only regenerate the shader from source if parameters have changed.
        m_haveCached = true;
        m_cachedRadius = filterRadius;
        QByteArray source = generateGaussianShader(filterRadius, m_singlePass);
        filter->setSource(source);
    }

    QRect targetRect = QRectF(src.rect()).translated(pos).adjusted(-actualRadius, -actualRadius, actualRadius, actualRadius).toAlignedRect();

    if (m_singlePass) {
        // prepare for updateUniforms
        m_textureSize = src.size();

        // ensure GL_LINEAR filtering is used
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);
        QBrush pixmapBrush = src;
        pixmapBrush.setTransform(QTransform::fromTranslate(pos.x(), pos.y()));
        painter->fillRect(targetRect, pixmapBrush);
        filter->removeFromPainter(painter);
    } else {
        QGLFramebufferObjectFormat format;
        format.setInternalTextureFormat(GLenum(src.hasAlphaChannel() ? GL_RGBA : GL_RGB));
        QGLFramebufferObject *fbo = qgl_fbo_pool()->acquire(targetRect.size(), format);

        if (!fbo)
            return false;

        // prepare for updateUniforms
        m_textureSize = src.size();

        // horizontal pass, to pixmap
        m_horizontalBlur = true;

        QPainter fboPainter(fbo);

        if (src.hasAlphaChannel()) {
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        // ensure GL_LINEAR filtering is used
        fboPainter.setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(&fboPainter);
        QBrush pixmapBrush = src;
        pixmapBrush.setTransform(QTransform::fromTranslate(actualRadius, actualRadius));
        fboPainter.fillRect(QRect(0, 0, targetRect.width(), targetRect.height()), pixmapBrush);
        filter->removeFromPainter(&fboPainter);
        fboPainter.end();

        QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(painter->paintEngine());

        // vertical pass, to painter
        m_horizontalBlur = false;
        m_textureSize = fbo->size();

        painter->save();
        // ensure GL_LINEAR filtering is used
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);
        engine->drawTexture(targetRect, fbo->texture(), fbo->size(), QRect(QPoint(), targetRect.size()).translated(0, fbo->height() - targetRect.height()));
        filter->removeFromPainter(painter);
        painter->restore();

        qgl_fbo_pool()->release(fbo);
    }

    return true;
}

void QGLPixmapBlurFilter::setUniforms(QGLShaderProgram *program)
{
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (m_hint == Qt::QualityHint) {
        if (m_singlePass)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 1.0 / m_textureSize.height());
        else if (m_horizontalBlur)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, 1.0 / m_textureSize.height());
    } else {
        qreal blur = radius() / qreal(m_cachedRadius);

        if (m_singlePass)
            program->setUniformValue("delta", blur / m_textureSize.width(), blur / m_textureSize.height());
        else if (m_horizontalBlur)
            program->setUniformValue("delta", blur / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, blur / m_textureSize.height());
    }
}

static inline qreal gaussian(qreal dx, qreal sigma)
{
    return exp(-dx * dx / (2 * sigma * sigma)) / (Q_2PI * sigma * sigma);
}

QByteArray QGLPixmapBlurFilter::generateGaussianShader(int radius, bool singlePass, bool dropShadow)
{
    Q_ASSERT(radius >= 1);

    radius = qMin(127, radius);

    static QCache<uint, QByteArray> shaderSourceCache;
    uint key = radius | (int(singlePass) << 7) | (int(dropShadow) << 8);
    QByteArray *cached = shaderSourceCache.object(key);
    if (cached)
        return *cached;

    QByteArray source;
    source.reserve(1000);
    source.append(qt_gl_texture_sampling_helper);

    source.append("uniform highp vec2      delta;\n");
    if (dropShadow)
        source.append("uniform mediump vec4    shadowColor;\n");
    source.append("lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords) {\n");

    QVector<qreal> sampleOffsets;
    QVector<qreal> weights;

    QVector<qreal> gaussianComponents;

    qreal sigma = radius / 1.65;

    qreal sum = 0;
    for (int i = -radius; i < radius; ++i) {
        float value = gaussian(i, sigma);
        gaussianComponents << value;
        sum += value;
    }

    // normalize
    for (int i = 0; i < gaussianComponents.size(); ++i)
        gaussianComponents[i] /= sum;

    for (int i = 0; i < gaussianComponents.size() - 1; i += 2) {
        qreal weight = gaussianComponents.at(i) + gaussianComponents.at(i + 1);
        qreal offset = i - radius + gaussianComponents.at(i + 1) / weight;

        sampleOffsets << offset;
        weights << weight;
    }

    int limit = sampleOffsets.size();
    if (singlePass)
        limit *= limit;

    QByteArray baseCoordinate = "srcCoords";

    for (int i = 0; i < limit; ++i) {
        QByteArray coordinate = baseCoordinate;

        qreal weight;
        if (singlePass) {
            const int xIndex = i % sampleOffsets.size();
            const int yIndex = i / sampleOffsets.size();

            const qreal deltaX = sampleOffsets.at(xIndex);
            const qreal deltaY = sampleOffsets.at(yIndex);
            weight = weights.at(xIndex) * weights.at(yIndex);

            if (!qFuzzyCompare(deltaX, deltaY)) {
                coordinate.append(" + vec2(delta.x * float(");
                coordinate.append(QByteArray::number(deltaX));
                coordinate.append("), delta.y * float(");
                coordinate.append(QByteArray::number(deltaY));
                coordinate.append("))");
            } else if (!qFuzzyIsNull(deltaX)) {
                coordinate.append(" + delta * float(");
                coordinate.append(QByteArray::number(deltaX));
                coordinate.append(")");
            }
        } else {
            const qreal delta = sampleOffsets.at(i);
            weight = weights.at(i);
            if (!qFuzzyIsNull(delta)) {
                coordinate.append(" + delta * float(");
                coordinate.append(QByteArray::number(delta));
                coordinate.append(")");
            }
        }

        if (i == 0) {
            if (dropShadow)
                source.append("    mediump float sample = ");
            else
                source.append("    mediump vec4 sample = ");
        } else {
            if (dropShadow)
                source.append("    sample += ");
            else
                source.append("    sample += ");
        }

        source.append("texture2D(src, ");
        source.append(coordinate);
        source.append(")");

        if (dropShadow)
            source.append(".a");

        if (!qFuzzyCompare(weight, qreal(1))) {
            source.append(" * float(");
            source.append(QByteArray::number(weight));
            source.append(");\n");
        } else {
            source.append(";\n");
        }
    }

    source.append("    return ");
    if (dropShadow)
        source.append("shadowColor * ");
    source.append("sample;\n");
    source.append("}\n");

    cached = new QByteArray(source);
    shaderSourceCache.insert(key, cached);

    return source;
}

QGLPixmapDropShadowFilter::QGLPixmapDropShadowFilter(Qt::RenderHint hint)
    : m_haveCached(false)
    , m_cachedRadius(0)
    , m_hint(hint)
{
}

bool QGLPixmapDropShadowFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    QGLPixmapDropShadowFilter *filter = const_cast<QGLPixmapDropShadowFilter *>(this);

    int actualRadius = qRound(blurRadius());
    int filterRadius = actualRadius;
    m_singlePass = filterRadius <= 3;

    if (!m_haveCached || filterRadius != m_cachedRadius) {
        // Only regenerate the shader from source if parameters have changed.
        m_haveCached = true;
        m_cachedRadius = filterRadius;
        QByteArray source = QGLPixmapBlurFilter::generateGaussianShader(filterRadius, m_singlePass, true);
        filter->setSource(source);
    }

    QRect targetRect = QRectF(src.rect()).translated(pos + offset()).adjusted(-actualRadius, -actualRadius, actualRadius, actualRadius).toAlignedRect();

    if (m_singlePass) {
        // prepare for updateUniforms
        m_textureSize = src.size();

        painter->save();
        // ensure GL_LINEAR filtering is used
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);
        QBrush pixmapBrush = src;
        pixmapBrush.setTransform(QTransform::fromTranslate(pos.x() + offset().x(), pos.y() + offset().y()));
        painter->fillRect(targetRect, pixmapBrush);
        filter->removeFromPainter(painter);
        painter->restore();
    } else {
        QGLFramebufferObjectFormat format;
        format.setInternalTextureFormat(GLenum(src.hasAlphaChannel() ? GL_RGBA : GL_RGB));
        QGLFramebufferObject *fbo = qgl_fbo_pool()->acquire(targetRect.size(), format);

        if (!fbo)
            return false;

        // prepare for updateUniforms
        m_textureSize = src.size();

        // horizontal pass, to pixmap
        m_horizontalBlur = true;

        QPainter fboPainter(fbo);

        if (src.hasAlphaChannel()) {
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        // ensure GL_LINEAR filtering is used
        fboPainter.setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(&fboPainter);
        QBrush pixmapBrush = src;
        pixmapBrush.setTransform(QTransform::fromTranslate(actualRadius, actualRadius));
        fboPainter.fillRect(QRect(0, 0, targetRect.width(), targetRect.height()), pixmapBrush);
        filter->removeFromPainter(&fboPainter);
        fboPainter.end();

        QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(painter->paintEngine());

        // vertical pass, to painter
        m_horizontalBlur = false;
        m_textureSize = fbo->size();

        painter->save();
        // ensure GL_LINEAR filtering is used
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        filter->setOnPainter(painter);
        engine->drawTexture(targetRect, fbo->texture(), fbo->size(), src.rect().translated(0, fbo->height() - src.height()));
        filter->removeFromPainter(painter);
        painter->restore();

        qgl_fbo_pool()->release(fbo);
    }

    // Now draw the actual pixmap over the top.
    painter->drawPixmap(pos, src, srcRect);

    return true;
}

void QGLPixmapDropShadowFilter::setUniforms(QGLShaderProgram *program)
{
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    QColor col = color();
    if (m_horizontalBlur && !m_singlePass) {
        program->setUniformValue("shadowColor", 1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        qreal alpha = col.alphaF();
        program->setUniformValue("shadowColor", col.redF() * alpha,
                                                col.greenF() * alpha,
                                                col.blueF() * alpha,
                                                alpha);
    }

    if (m_hint == Qt::QualityHint) {
        if (m_singlePass)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 1.0 / m_textureSize.height());
        else if (m_horizontalBlur)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, 1.0 / m_textureSize.height());
    } else {
        qreal blur = blurRadius() / qreal(m_cachedRadius);

        if (m_singlePass)
            program->setUniformValue("delta", blur / m_textureSize.width(), blur / m_textureSize.height());
        else if (m_horizontalBlur)
            program->setUniformValue("delta", blur / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, blur / m_textureSize.height());
    }
}

QT_END_NAMESPACE
