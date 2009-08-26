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
** contact the sales department at http://qt.nokia.com/contact.
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
    const_cast<QGLContext *>(QGLContext::currentContext())->d_func()->bindTexture(src, GL_TEXTURE_2D, GL_RGBA, true, false);
}

void QGLPixmapFilterBase::drawImpl(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF& source) const
{
    processGL(painter, pos, src, source);
}

class QGLPixmapColorizeFilter: public QGLPixmapFilter<QPixmapColorizeFilter>
{
public:
    QGLPixmapColorizeFilter();

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &pixmap, const QRectF &srcRect) const;

private:
    mutable QGLShaderProgram m_program;
    int m_colorUniform;
};

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

class QGLPixmapBlurFilter : public QGLCustomShaderStage, public QGLPixmapFilter<QPixmapBlurFilter>
{
public:
    QGLPixmapBlurFilter();
    ~QGLPixmapBlurFilter();

    void setUniforms(QGLShaderProgram *program);

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:
    static QByteArray generateBlurShader(int radius, bool gaussianBlur);

    mutable QGLShader *m_shader;

    mutable QSize m_textureSize;

    QGLShaderProgram *m_program;
};

extern QGLWidget *qt_gl_share_widget();

QPixmapFilter *QGLContextPrivate::createPixmapFilter(int type) const
{
    switch (type) {
    case QPixmapFilter::ColorizeFilter:
        return new QGLPixmapColorizeFilter;

    case QPixmapFilter::BlurFilter:
        return new QGLPixmapBlurFilter;

    case QPixmapFilter::ConvolutionFilter:
        return new QGLPixmapConvolutionFilter;

    default:
        return 0;
        break;
    }
    return 0;
}

extern void qt_add_rect_to_array(const QRectF &r, q_vertexType *array);
extern void qt_add_texcoords_to_array(qreal x1, qreal y1, qreal x2, qreal y2, q_vertexType *array);

static void qgl_drawTexture(const QRectF &rect, int tx_width, int tx_height, const QRectF & src)
{
#ifndef QT_OPENGL_ES_2  // XXX: needs to be ported
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
#endif
}

static const char *qt_gl_colorize_filter =
        "uniform sampler2D texture;"
        "uniform vec3 color;"
        "void main(void)"
        "{"
        "        vec2 coords = gl_TexCoord[0].st;"
        "        vec4 src = texture2D(texture, coords);"
        "        float gray = dot(src.rgb, vec3(0.212671, 0.715160, 0.072169));"
        "        vec3 colorizeed = 1.0-((1.0-gray)*(1.0-color));"
        "        gl_FragColor = vec4(colorizeed, src.a);"
        "}";

QGLPixmapColorizeFilter::QGLPixmapColorizeFilter()
{
    m_program.addShader(QGLShader::FragmentShader, qt_gl_colorize_filter);
    m_program.link();
    m_program.enable();
    m_program.setUniformValue(m_program.uniformLocation("texture"), GLint(0)); // GL_TEXTURE_0
    m_colorUniform = m_program.uniformLocation("color");
}

bool QGLPixmapColorizeFilter::processGL(QPainter *, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    bindTexture(src);

    QColor col = color();
    m_program.enable();
    m_program.setUniformValue(m_colorUniform, col.redF(), col.greenF(), col.blueF());

    QRectF target = (srcRect.isNull() ? QRectF(src.rect()) : srcRect).translated(pos);
    qgl_drawTexture(target, src.width(), src.height(), srcRect);
    m_program.disable();

    return true;
}

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

QGLPixmapBlurFilter::QGLPixmapBlurFilter()
{
}

QGLPixmapBlurFilter::~QGLPixmapBlurFilter()
{
}

bool QGLPixmapBlurFilter::processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &) const
{
    QGLPixmapBlurFilter *filter = const_cast<QGLPixmapBlurFilter *>(this);
    filter->setSource(generateBlurShader(radius(), quality() == Qt::SmoothTransformation));

    QGLFramebufferObjectFormat format;
    format.setInternalFormat(src.hasAlphaChannel() ? GL_RGBA : GL_RGB);
    QGLFramebufferObject *fbo = qgl_fbo_pool()->acquire(src.size(), format);

    if (!fbo)
        return false;

    glBindTexture(GL_TEXTURE_2D, fbo->texture());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    QGL2PaintEngineEx *engine = static_cast<QGL2PaintEngineEx *>(painter->paintEngine());

    painter->save();

    // ensure GL_LINEAR filtering is used
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    // prepare for updateUniforms
    m_textureSize = src.size();

    // first pass, to fbo
    fbo->bind();
    if (src.hasAlphaChannel()) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    filter->setOnPainter(painter);

    QTransform transform = engine->state()->matrix;
    if (!transform.isIdentity()) {
        engine->state()->matrix = QTransform();
        engine->transformChanged();
    }

    engine->drawPixmap(src.rect().translated(0, painter->device()->height() - fbo->height()),
                       src, src.rect());

    if (!transform.isIdentity()) {
        engine->state()->matrix = transform;
        engine->transformChanged();
    }

    fbo->release();

    // second pass, to widget
    m_program->setUniformValue("delta", 0.0, 1.0);
    engine->drawTexture(src.rect().translated(pos.x(), pos.y()), fbo->texture(), fbo->size(), src.rect().translated(0, fbo->height() - src.height()));
    filter->removeFromPainter(painter);

    painter->restore();

    qgl_fbo_pool()->release(fbo);

    return true;
}

void QGLPixmapBlurFilter::setUniforms(QGLShaderProgram *program)
{
    program->setUniformValue("invTextureSize", 1.0 / m_textureSize.width(), 1.0 / m_textureSize.height());
    program->setUniformValue("delta", 1.0, 0.0);

    m_program = program;
}

static inline qreal gaussian(qreal dx, qreal sigma)
{
    return exp(-dx * dx / (2 * sigma * sigma)) / (Q_2PI * sigma * sigma);
}

QByteArray QGLPixmapBlurFilter::generateBlurShader(int radius, bool gaussianBlur)
{
    Q_ASSERT(radius >= 1);

    QByteArray source;
    source.reserve(1000);

    source.append("uniform highp vec2      invTextureSize;\n");

    bool separateXY = true;
    bool clip = false;

    if (separateXY) {
        source.append("uniform highp vec2      delta;\n");

        if (clip)
            source.append("uniform highp vec2      clip;\n");
    } else if (clip) {
        source.append("uniform highp vec4      clip;\n");
    }

    source.append("lowp vec4 customShader(lowp sampler2D src, highp vec2 srcCoords) {\n");

    QVector<qreal> sampleOffsets;
    QVector<qreal> weights;

    if (gaussianBlur) {
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
    } else {
        for (int i = 0; i < radius; ++i) {
            sampleOffsets << 2 * i - radius + 0.5;
            weights << qreal(1);
        }
        sampleOffsets << radius;
        weights << qreal(0.5);
    }

    int currentVariable = 1;
    source.append("    mediump vec4 sample = vec4(0.0);\n");
    source.append("    mediump vec2 coord;\n");

    qreal weightSum = 0;
    if (separateXY) {
        source.append("    mediump float c;\n");
        for (int i = 0; i < sampleOffsets.size(); ++i) {
            qreal delta = sampleOffsets.at(i);

            ++currentVariable;

            QByteArray coordinate = "srcCoords";
            if (delta != qreal(0)) {
                coordinate.append(" + invTextureSize * delta * float(");
                coordinate.append(QByteArray::number(delta));
                coordinate.append(")");
            }

            source.append("    coord = ");
            source.append(coordinate);
            source.append(";\n");

            if (clip) {
                source.append("    c = dot(coord, delta);\n");
                source.append("    if (c > clip.x && c < clip.y)\n    ");
            }

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
    } else {
        for (int y = 0; y < sampleOffsets.size(); ++y) {
            for (int x = 0; x < sampleOffsets.size(); ++x) {
                QByteArray coordinate = "srcCoords";

                qreal dx = sampleOffsets.at(x);
                qreal dy = sampleOffsets.at(y);

                if (dx != qreal(0) || dy != qreal(0)) {
                    coordinate.append(" + invTextureSize * vec2(float(");
                    coordinate.append(QByteArray::number(dx));
                    coordinate.append("), float(");
                    coordinate.append(QByteArray::number(dy));
                    coordinate.append("))");
                }

                source.append("    coord = ");
                source.append(coordinate);
                source.append(";\n");

                if (clip)
                    source.append("    if (coord.x > clip.x && coord.x < clip.y && coord.y > clip.z && coord.y < clip.w)\n    ");

                source.append("    sample += texture2D(src, coord)");

                ++currentVariable;

                weightSum += weights.at(x) * weights.at(y);
                if ((weights.at(x) != qreal(1) || weights.at(y) != qreal(1))) {
                    source.append(" * float(");
                    source.append(QByteArray::number(weights.at(x) * weights.at(y)));
                    source.append(");\n");
                } else {
                    source.append(";\n");
                }
            }
        }
    }

    source.append("    return ");
    if (!gaussianBlur) {
        source.append("float(");
        if (separateXY)
            source.append(QByteArray::number(1 / weightSum));
        else
            source.append(QByteArray::number(1 / weightSum));
        source.append(") * ");
    }
    source.append("sample;\n");
    source.append("}\n");

    return source;
}

QT_END_NAMESPACE
