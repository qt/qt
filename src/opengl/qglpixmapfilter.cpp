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

    static QByteArray generateGaussianShader(int radius, bool dropShadow = false);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:

    mutable QSize m_textureSize;
    mutable bool m_horizontalBlur;

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

static const char *qt_gl_blur_filter_fast =
    "const int samples = 9;"
    "uniform mediump vec2 delta;"
    "lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords) {"
    "    mediump vec4 color = vec4(0.0, 0.0, 0.0, 0.0);"
    "    mediump float offset = (float(samples) - 1.0) / 2.0;"
    "    for (int i = 0; i < samples; i++) {"
    "        mediump vec2 coord = srcCoords + delta * (offset - float(i)) / offset;"
    "        color += texture2D(src, coord);"
    "    }"
    "    return color * (1.0 / float(samples));"
    "}";

static const char *qt_gl_drop_shadow_filter_fast =
    "const int samples = 9;"
    "uniform mediump vec2 delta;"
    "uniform mediump vec4 shadowColor;"
    "lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords) {"
    "    mediump vec4 color = vec4(0.0, 0.0, 0.0, 0.0);"
    "    mediump float offset = (float(samples) - 1.0) / 2.0;"
    "    for (int i = 0; i < samples; i++) {"
    "        mediump vec2 coord = srcCoords + delta * (offset - float(i)) / offset;"
    "        color += texture2D(src, coord).a * shadowColor;"
    "    }"
    "    return color * (1.0 / float(samples));"
    "}";

QGLPixmapBlurFilter::QGLPixmapBlurFilter(Qt::RenderHint hint)
    : m_haveCached(false)
    , m_cachedRadius(5)
    , m_hint(hint)
{
    if (hint == Qt::PerformanceHint) {
        QGLPixmapBlurFilter *filter = const_cast<QGLPixmapBlurFilter *>(this);
        filter->setSource(qt_gl_blur_filter_fast);
        m_haveCached = true;
    }
}

bool QGLPixmapBlurFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &) const
{
    QGLPixmapBlurFilter *filter = const_cast<QGLPixmapBlurFilter *>(this);

    int radius = this->radius();
    if (!m_haveCached || (m_hint == Qt::QualityHint && radius != m_cachedRadius)) {
        // Only regenerate the shader from source if parameters have changed.
        m_haveCached = true;
        m_cachedRadius = radius;
        filter->setSource(generateGaussianShader(radius));
    }

    QGLFramebufferObjectFormat format;
    format.setInternalTextureFormat(GLenum(src.hasAlphaChannel() ? GL_RGBA : GL_RGB));
    QGLFramebufferObject *fbo = qgl_fbo_pool()->acquire(src.size(), format);

    if (!fbo)
        return false;

    glBindTexture(GL_TEXTURE_2D, fbo->texture());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

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
    fboPainter.drawPixmap(0, 0, src);
    filter->removeFromPainter(&fboPainter);
    fboPainter.end();

    QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(painter->paintEngine());

    // vertical pass, to painter
    m_horizontalBlur = false;

    painter->save();
    // ensure GL_LINEAR filtering is used
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    filter->setOnPainter(painter);
    engine->drawTexture(src.rect().translated(pos.x(), pos.y()), fbo->texture(), fbo->size(), src.rect().translated(0, fbo->height() - src.height()));
    filter->removeFromPainter(painter);
    painter->restore();

    qgl_fbo_pool()->release(fbo);

    return true;
}

void QGLPixmapBlurFilter::setUniforms(QGLShaderProgram *program)
{
    if (m_hint == Qt::QualityHint) {
        if (m_horizontalBlur)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, 1.0 / m_textureSize.height());
    } else {
        // 1.4 is chosen to most closely match the blurriness of the gaussian blur
        // at low radii
        qreal blur = radius() / 1.4f;

        if (m_horizontalBlur)
            program->setUniformValue("delta", blur / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, blur / m_textureSize.height());
    }
}

static inline qreal gaussian(qreal dx, qreal sigma)
{
    return exp(-dx * dx / (2 * sigma * sigma)) / (Q_2PI * sigma * sigma);
}

QByteArray QGLPixmapBlurFilter::generateGaussianShader(int radius, bool dropShadow)
{
    Q_ASSERT(radius >= 1);

    QByteArray source;
    source.reserve(1000);

    source.append("uniform highp vec2      delta;\n");
    if (dropShadow)
        source.append("uniform mediump vec4    shadowColor;\n");
    source.append("lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords) {\n");

    QVector<qreal> sampleOffsets;
    QVector<qreal> weights;

    QVector<qreal> gaussianComponents;

    qreal sigma = radius / 1.65;

    qreal sum = 0;
    for (int i = -radius; i <= radius; ++i) {
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

    // odd size ?
    if (gaussianComponents.size() & 1) {
        sampleOffsets << radius;
        weights << gaussianComponents.last();
    }

    int currentVariable = 1;
    source.append("    mediump vec4 sample = vec4(0.0);\n");
    source.append("    mediump vec2 coord;\n");

    qreal weightSum = 0;
    source.append("    mediump float c;\n");
    for (int i = 0; i < sampleOffsets.size(); ++i) {
        qreal delta = sampleOffsets.at(i);

        ++currentVariable;

        QByteArray coordinate = "srcCoords";
        if (delta != qreal(0)) {
            coordinate.append(" + delta * float(");
            coordinate.append(QByteArray::number(delta));
            coordinate.append(")");
        }

        source.append("    coord = ");
        source.append(coordinate);
        source.append(";\n");

        if (dropShadow)
            source.append("    sample += texture2D(src, coord).a * shadowColor");
        else
            source.append("    sample += texture2D(src, coord)");

        weightSum += weights.at(i);
        if (weights.at(i) != qreal(1)) {
            source.append(" * float(");
            source.append(QByteArray::number(weights.at(i)));
            source.append(");\n");
        } else {
            source.append(";\n");
        }
    }

    source.append("    return ");
    source.append("sample;\n");
    source.append("}\n");

    return source;
}

QGLPixmapDropShadowFilter::QGLPixmapDropShadowFilter(Qt::RenderHint hint)
    : m_haveCached(false)
    , m_cachedRadius(5)
    , m_hint(hint)
{
    if (hint == Qt::PerformanceHint) {
        QGLPixmapDropShadowFilter *filter = const_cast<QGLPixmapDropShadowFilter *>(this);
        filter->setSource(qt_gl_drop_shadow_filter_fast);
        m_haveCached = true;
    }
}

bool QGLPixmapDropShadowFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    QGLPixmapDropShadowFilter *filter = const_cast<QGLPixmapDropShadowFilter *>(this);

    int radius = this->blurRadius();
    if (!m_haveCached || (m_hint == Qt::QualityHint && radius != m_cachedRadius)) {
        // Only regenerate the shader from source if parameters have changed.
        m_haveCached = true;
        m_cachedRadius = radius;
        filter->setSource(QGLPixmapBlurFilter::generateGaussianShader(radius, true));
    }

    QGLFramebufferObjectFormat format;
    format.setInternalTextureFormat(GLenum(src.hasAlphaChannel() ? GL_RGBA : GL_RGB));
    QGLFramebufferObject *fbo = qgl_fbo_pool()->acquire(src.size(), format);

    if (!fbo)
        return false;

    glBindTexture(GL_TEXTURE_2D, fbo->texture());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

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
    fboPainter.drawPixmap(0, 0, src);
    filter->removeFromPainter(&fboPainter);
    fboPainter.end();

    QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(painter->paintEngine());

    // vertical pass, to painter
    m_horizontalBlur = false;

    painter->save();
    // ensure GL_LINEAR filtering is used
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    filter->setOnPainter(painter);
    QPointF ofs = offset();
    engine->drawTexture(src.rect().translated(pos.x() + ofs.x(), pos.y() + ofs.y()), fbo->texture(), fbo->size(), src.rect().translated(0, fbo->height() - src.height()));
    filter->removeFromPainter(painter);
    painter->restore();

    qgl_fbo_pool()->release(fbo);

    // Now draw the actual pixmap over the top.
    painter->drawPixmap(pos, src, srcRect);

    return true;
}

void QGLPixmapDropShadowFilter::setUniforms(QGLShaderProgram *program)
{
    QColor col = color();
    if (m_horizontalBlur) {
        program->setUniformValue("shadowColor", 1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        qreal alpha = col.alphaF();
        program->setUniformValue("shadowColor", col.redF() * alpha,
                                                col.greenF() * alpha,
                                                col.blueF() * alpha,
                                                alpha);
    }
    if (m_hint == Qt::QualityHint) {
        if (m_horizontalBlur)
            program->setUniformValue("delta", 1.0 / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, 1.0 / m_textureSize.height());
    } else {
        // 1.4 is chosen to most closely match the blurriness of the gaussian blur
        // at low radii
        qreal blur = blurRadius() / 1.4f;

        if (m_horizontalBlur)
            program->setUniformValue("delta", blur / m_textureSize.width(), 0.0);
        else
            program->setUniformValue("delta", 0.0, blur / m_textureSize.height());
    }
}

QT_END_NAMESPACE
