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

#ifndef QT_OPENGL_ES_2

class QGLPixmapConvolutionFilter: public QGLPixmapFilter<QPixmapConvolutionFilter>
{
public:
    QGLPixmapConvolutionFilter();
    ~QGLPixmapConvolutionFilter();

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:
    QByteArray generateConvolutionShader() const;

    mutable QGLShaderProgram *m_program;
    mutable int m_scaleUniform;
    mutable int m_matrixUniform;

    mutable int m_kernelWidth;
    mutable int m_kernelHeight;
};

#endif

class QGLPixmapBlurFilter : public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapBlurFilter>
{
public:
    QGLPixmapBlurFilter(QPixmapBlurFilter::BlurHint hint);

    void setUniforms(QGLShaderProgram *program);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:
    static QByteArray generateGaussianShader(int radius);

    mutable QSize m_textureSize;
    mutable bool m_horizontalBlur;

    mutable bool m_haveCached;
    mutable int m_cachedRadius;
    mutable QPixmapBlurFilter::BlurHint m_hint;
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
        if (proto->blurHint() == QPixmapBlurFilter::PerformanceHint || proto->radius() <= 5) {
            if (!d->fastBlurFilter)
                d->fastBlurFilter.reset(new QGLPixmapBlurFilter(QPixmapBlurFilter::PerformanceHint));
            return d->fastBlurFilter.data();
        }
        if (!d->blurFilter)
            d->blurFilter.reset(new QGLPixmapBlurFilter(QPixmapBlurFilter::QualityHint));
        return d->blurFilter.data();
        }

#ifndef QT_OPENGL_ES_2
    case QPixmapFilter::ConvolutionFilter:
        if (!d->convolutionFilter)
            d->convolutionFilter.reset(new QGLPixmapConvolutionFilter);
        return d->convolutionFilter.data();
#endif

    default: break;
    }
    return QPaintEngineEx::pixmapFilter(type, prototype);
}

#ifndef QT_OPENGL_ES_2  // XXX: needs to be ported

extern void qt_add_rect_to_array(const QRectF &r, q_vertexType *array);
extern void qt_add_texcoords_to_array(qreal x1, qreal y1, qreal x2, qreal y2, q_vertexType *array);

static void qgl_drawTexture(const QRectF &rect, int tx_width, int tx_height, const QRectF & src)
{
#ifndef QT_OPENGL_ES
    glPushAttrib(GL_CURRENT_BIT);
#endif
    qreal x1, x2, y1, y2;

    x1 = src.x() / tx_width;
    x2 = x1 + src.width() / tx_width;
    y1 = 1.0 - ((src.y() / tx_height) + (src.height() / tx_height));
    y2 = 1.0 - (src.y() / tx_height);

    q_vertexType vertexArray[4*2];
    q_vertexType texCoordArray[4*2];

    qt_add_rect_to_array(rect, vertexArray);
    qt_add_texcoords_to_array(x1, y2, x2, y1, texCoordArray);

    glVertexPointer(2, q_vertexTypeEnum, 0, vertexArray);
    glTexCoordPointer(2, q_vertexTypeEnum, 0, texCoordArray);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

#ifndef QT_OPENGL_ES
    glPopAttrib();
#endif
}

#endif // !QT_OPENGL_ES_2

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

#ifndef QT_OPENGL_ES_2

// generates convolution filter code for arbitrary sized kernel
QByteArray QGLPixmapConvolutionFilter::generateConvolutionShader() const {
    QByteArray code;
    code.append("uniform sampler2D texture;\n"
                "uniform vec2 inv_texture_size;\n"
                "uniform float matrix[");
    code.append(QByteArray::number(m_kernelWidth * m_kernelHeight));
    code.append("];\n"
                "vec2 offset[");
    code.append(QByteArray::number(m_kernelWidth*m_kernelHeight));
    code.append("];\n"
                "void main(void) {\n");

    for(int y = 0; y < m_kernelHeight; y++) {
        for(int x = 0; x < m_kernelWidth; x++) {
            code.append("  offset[");
            code.append(QByteArray::number(y * m_kernelWidth + x));
            code.append("] = vec2(inv_texture_size.x * ");
            code.append(QByteArray::number(x-(int)(m_kernelWidth/2)));
            code.append(".0, inv_texture_size.y * ");
            code.append(QByteArray::number((int)(m_kernelHeight/2)-y));
            code.append(".0);\n");
        }
    }

    code.append("  int i = 0;\n"
                "  vec2 coords = gl_TexCoord[0].st;\n"
                "  vec4 sum = vec4(0.0);\n"
                "  for (i = 0; i < ");
    code.append(QByteArray::number(m_kernelWidth * m_kernelHeight));
    code.append("; i++) {\n"
                "    vec4 tmp = texture2D(texture,coords+offset[i]);\n"
                "    sum += matrix[i] * tmp;\n"
                "  }\n"
                "  gl_FragColor = sum;\n"
                "}");
    return code;
}

QGLPixmapConvolutionFilter::QGLPixmapConvolutionFilter()
    : m_program(0)
    , m_scaleUniform(0)
    , m_matrixUniform(0)
    , m_kernelWidth(0)
    , m_kernelHeight(0)
{
}

QGLPixmapConvolutionFilter::~QGLPixmapConvolutionFilter()
{
    delete m_program;
}

bool QGLPixmapConvolutionFilter::processGL(QPainter *, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    QRectF target = (srcRect.isNull() ? QRectF(src.rect()) : srcRect).translated(pos);

    bindTexture(src);
#ifdef GL_CLAMP
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
    if (!m_program || m_kernelWidth != columns() || m_kernelHeight != rows()) {
        delete m_program;

        m_kernelWidth = columns();
        m_kernelHeight = rows();

        QByteArray code = generateConvolutionShader();
        m_program = new QGLShaderProgram();
        m_program->addShader(QGLShader::FragmentShader, code);
        m_program->link();
        m_scaleUniform = m_program->uniformLocation("inv_texture_size");
        m_matrixUniform = m_program->uniformLocation("matrix");
    }

    const qreal *kernel = convolutionKernel();
    GLfloat *conv = new GLfloat[m_kernelWidth * m_kernelHeight];
    for(int i = 0; i < m_kernelWidth * m_kernelHeight; ++i)
        conv[i] = kernel[i];

    const qreal iw = 1.0 / src.width();
    const qreal ih = 1.0 / src.height();
    m_program->enable();
    m_program->setUniformValue(m_scaleUniform, iw, ih);
    m_program->setUniformValueArray(m_matrixUniform, conv, m_kernelWidth * m_kernelHeight, 1);

    qgl_drawTexture(target, src.width(), src.height(), boundingRectFor(srcRect));
    m_program->disable();
    return true;
}

#endif // !QT_OPENGL_ES_2

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

QGLPixmapBlurFilter::QGLPixmapBlurFilter(QPixmapBlurFilter::BlurHint hint)
    : m_haveCached(false)
    , m_cachedRadius(5)
    , m_hint(hint)
{
    if (hint == PerformanceHint) {
        QGLPixmapBlurFilter *filter = const_cast<QGLPixmapBlurFilter *>(this);
        filter->setSource(qt_gl_blur_filter_fast);
        m_haveCached = true;
    }
}

bool QGLPixmapBlurFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &) const
{
    QGLPixmapBlurFilter *filter = const_cast<QGLPixmapBlurFilter *>(this);

    int radius = this->radius();
    if (!m_haveCached || (m_hint == QualityHint && radius != m_cachedRadius)) {
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
    if (m_hint == QualityHint) {
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

QByteArray QGLPixmapBlurFilter::generateGaussianShader(int radius)
{
    Q_ASSERT(radius >= 1);

    QByteArray source;
    source.reserve(1000);

    source.append("uniform highp vec2      delta;\n");
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

QT_END_NAMESPACE
