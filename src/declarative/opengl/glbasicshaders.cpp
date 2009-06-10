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

#include "glbasicshaders.h"
#include <QDebug>
#include <QColor>


QT_BEGIN_NAMESPACE
SingleTextureVertexOpacityShader::SingleTextureVertexOpacityShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            attribute highp vec4 myVertex;\
            attribute lowp float myOpacity;\
            attribute mediump vec4 myUV;\
            uniform mediump mat4 myPMVMatrix;\
            varying mediump vec2 myTexCoord;\
            varying lowp float myFragOpacity;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex;\
                myTexCoord = myUV.st;\
                myFragOpacity = myOpacity;\
            }"
            );

    frag.compile("\
            uniform sampler2D sampler2d;\
            varying mediump vec2 myTexCoord;\
            varying lowp float myFragOpacity;\
            void main(void)\
            {\
                mediump vec4 frag = texture2D(sampler2d,myTexCoord);\
                gl_FragColor = vec4(frag.rgb, frag.a * myFragOpacity);\
            }"
            );

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
    bindAttributeLocation("myUV", TextureCoords);
    bindAttributeLocation("myOpacity", OpacityCoords);
}

bool SingleTextureVertexOpacityShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    enable();
    setUniformValue("sampler2d", 0);
    disable();
    return true;
}

void SingleTextureVertexOpacityShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

BlurTextureShader::BlurTextureShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            attribute highp vec4 myVertex;\
            attribute mediump vec4 myUV;\
            uniform mediump mat4 myPMVMatrix;\
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex;\
                myTexCoord = myUV.st;\
            }"
            );

#if 0
    frag.compile("\
            uniform sampler2D sampler2d;\
            uniform bool horizontal; \
            uniform mediump float blurStep; \
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                mediump vec4 accum = vec4(0, 0, 0, 0); \
                mediump vec2 offset; \
                if (horizontal) \
                    offset = vec2(blurStep, 0); \
                else \
                    offset = vec2(0, blurStep); \
                accum += texture2D(sampler2d, myTexCoord + 2.0 * offset); \
                accum += 2.0 * texture2D(sampler2d, myTexCoord + 1.0 * offset); \
                accum += 4.0 * texture2D(sampler2d, myTexCoord + 0.0 * offset); \
                accum += 2.0 * texture2D(sampler2d, myTexCoord - 1.0 * offset); \
                accum += texture2D(sampler2d, myTexCoord - 2.0 * offset); \
                gl_FragColor = accum / 10.0; \
            }"
            );
#else
    frag.compile("\
            uniform sampler2D sampler2d;\
            uniform bool horizontal; \
            uniform mediump float blurStep; \
            uniform int blurSteps; \
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                mediump vec4 accum = vec4(0, 0, 0, 0); \
                mediump vec2 offset; \
                if (horizontal) \
                    offset = vec2(blurStep, 0); \
                else \
                    offset = vec2(0, blurStep); \
                mediump float sum = 0.0; \
                for (int ii = 0; ii < blurSteps; ++ii) { \
                    mediump float frac = float(blurSteps - ii) / float(blurSteps); \
                    mediump vec2 coord = myTexCoord + -float(ii) * offset; \
                    if (coord.x >= 0.0 && coord.y >= 0.0 && coord.y <= 1.0 && coord.x <=1.0) \
                        accum += texture2D(sampler2d, coord) * frac; \
                    sum += frac; \
                } \
                for (int ii = 1; ii < blurSteps; ++ii) { \
                    mediump float frac = float(blurSteps - ii) / float(blurSteps); \
                    mediump vec2 coord = myTexCoord + float(ii) * offset; \
                    if (coord.x <= 1.0 && coord.y <= 1.0 && coord.x >= 0.0 && coord.y >= 0.0) \
                        accum += texture2D(sampler2d, coord) * frac; \
                    sum += frac; \
                } \
                gl_FragColor = accum / sum; \
            }"
            );
#endif

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
    bindAttributeLocation("myUV", TextureCoords);
}

bool BlurTextureShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    mode = uniformLocation("horizontal");
    step = uniformLocation("blurStep");
    steps = uniformLocation("blurSteps");
    enable();
    setUniformValue("sampler2d", 0);
    disable();
    return true;
}

void BlurTextureShader::setStep(float f)
{
    setUniformValue(step, f);
}

void BlurTextureShader::setSteps(int s)
{
    setUniformValue(steps, s);
}

void BlurTextureShader::setMode(Mode m)
{
    if (m == Horizontal)
        setUniformValue(mode, 1);
    else
        setUniformValue(mode, 0);
}

void BlurTextureShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

DualTextureBlendShader::DualTextureBlendShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            attribute highp vec4 myVertex;\
            attribute mediump vec4 myUV;\
            attribute mediump vec4 myBlendUV;\
            uniform mediump mat4 myPMVMatrix;\
            varying mediump vec2 myTexCoord;\
            varying mediump vec2 myBlendTexCoord;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex;\
                myTexCoord = myUV.st;\
                myBlendTexCoord = myBlendUV.st;\
            }"
            );

    frag.compile("\
            uniform sampler2D sampler2d;\
            uniform sampler2D sampler2dBlend;\
            uniform lowp float myOpacity;\
            uniform lowp float myBlend; \
            varying mediump vec2 myTexCoord;\
            varying mediump vec2 myBlendTexCoord;\
            void main(void)\
            {\
                mediump vec4 tex = texture2D(sampler2d,myTexCoord);\
                mediump vec4 blendtex = texture2D(sampler2dBlend, myBlendTexCoord);\
                gl_FragColor = mix(tex, blendtex, myBlend) * myOpacity; \
            }"
            );

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
    bindAttributeLocation("myUV", TextureCoords);
    bindAttributeLocation("myBlendUV", BlendTextureCoords);
}

bool DualTextureBlendShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    opacity = uniformLocation("myOpacity");
    blend = uniformLocation("myBlend");
    enable();
    setUniformValue("sampler2d", 0);
    setUniformValue("sampler2dBlend", 1);
    disable();
    return true;
}

void DualTextureBlendShader::setOpacity(GLfloat o)
{
    setUniformValue(opacity, o);
}

void DualTextureBlendShader::setBlend(GLfloat b)
{
    setUniformValue(blend, b);
}

void DualTextureBlendShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

DualTextureAddShader::DualTextureAddShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            attribute highp vec4 myVertex;\
            attribute mediump vec4 myUV;\
            attribute mediump vec4 myAddUV;\
            uniform mediump mat4 myPMVMatrix;\
            varying mediump vec2 myTexCoord;\
            varying mediump vec2 myAddTexCoord;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex;\
                myTexCoord = myUV.st;\
                myAddTexCoord = myAddUV.st;\
            }"
            );

    frag.compile("\
            uniform sampler2D sampler2d;\
            uniform sampler2D sampler2dAdd;\
            uniform lowp float myOpacity;\
            varying mediump vec2 myTexCoord;\
            varying mediump vec2 myAddTexCoord;\
            void main(void)\
            {\
                mediump vec4 tex = texture2D(sampler2d,myTexCoord);\
                mediump vec4 addtex = texture2D(sampler2dAdd, myAddTexCoord);\
                tex = tex + vec4(addtex.rgb * addtex.a * tex.a, 0); \
                tex = min(tex, vec4(1, 1, 1, 1)); \
                gl_FragColor = vec4(tex.rgb, tex.a) * myOpacity;\
            }"
            );

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
    bindAttributeLocation("myUV", TextureCoords);
    bindAttributeLocation("myAddUV", AddTextureCoords);
}

void DualTextureAddShader::setOpacity(GLfloat f)
{
    setUniformValue(opacity, f);
}

void DualTextureAddShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

bool DualTextureAddShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    opacity = uniformLocation("myOpacity");
    enable();
    setUniformValue("sampler2d", 0);
    setUniformValue("sampler2dAdd", 1);
    disable();
    return true;
}

SingleTextureShader::SingleTextureShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            attribute highp vec4 myVertex;\
            attribute mediump vec4 myUV;\
            uniform mediump mat4 myPMVMatrix;\
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex;\
                myTexCoord = myUV.st;\
            }"
            );

    frag.compile("\
            uniform sampler2D sampler2d;\
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                gl_FragColor = texture2D(sampler2d,myTexCoord);\
            }"
            );

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
    bindAttributeLocation("myUV", TextureCoords);
}

bool SingleTextureShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    enable();
    setUniformValue("sampler2d", 0);
    disable();
    return true;
}

void SingleTextureShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

ConstantColorShader::ConstantColorShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            uniform mediump mat4 myPMVMatrix;\
            attribute highp vec4 myVertex;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex; \
            }"
            );

    frag.compile("\
            uniform lowp vec4 myColor;\
            void main(void)\
            {\
                gl_FragColor = myColor;\
            }"
            );

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
}

void ConstantColorShader::setColor(const QColor &c)
{
    setUniformValue(color, c);
}

void ConstantColorShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

bool ConstantColorShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    color = uniformLocation("myColor");
    return true;
}

ColorShader::ColorShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            uniform mediump mat4 myPMVMatrix;\
            attribute highp vec4 myVertex;\
            attribute lowp vec4 myColors;\
            varying lowp vec4 myColor;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex; \
                myColor = myColors; \
            }"
            );

    frag.compile("\
            varying lowp vec4 myColor;\
            void main(void)\
            {\
                gl_FragColor = myColor;\
            }"
            );

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
    bindAttributeLocation("myColors", Colors);
}

void ColorShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

bool ColorShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    return true;
}

class GLBasicShadersPrivate
{
public:
    GLBasicShadersPrivate();
    ~GLBasicShadersPrivate();

    BlurTextureShader *blurTexture;
    SingleTextureShader *singleTexture;
    SingleTextureOpacityShader *singleTextureOpacity;
    DualTextureAddShader *dualTextureAdd;
    SingleTextureShadowShader *singleTextureShadow;
    SingleTextureVertexOpacityShader *singleTextureVertexOpacity;
    ConstantColorShader *constantColor;
    ColorShader *color;
};

GLBasicShadersPrivate::GLBasicShadersPrivate()
: blurTexture(0), singleTexture(0), singleTextureOpacity(0), 
  dualTextureAdd(0), singleTextureShadow(0), singleTextureVertexOpacity(0), 
  constantColor(0), color(0)
{
}

GLBasicShadersPrivate::~GLBasicShadersPrivate()
{
    delete blurTexture;
    delete singleTexture;
    delete singleTextureOpacity; 
    delete dualTextureAdd;
    delete singleTextureVertexOpacity; 
    delete singleTextureShadow;
    delete constantColor;
    delete color;
}

GLBasicShaders::GLBasicShaders()
: d(new GLBasicShadersPrivate)
{
}

GLBasicShaders::~GLBasicShaders()
{
    delete d;
}

BlurTextureShader *GLBasicShaders::blurTexture()
{
    if (!d->blurTexture) d->blurTexture = new BlurTextureShader();
    return d->blurTexture;
}

SingleTextureShader *GLBasicShaders::singleTexture()
{
    if (!d->singleTexture) d->singleTexture = new SingleTextureShader();
    return d->singleTexture;
}

SingleTextureOpacityShader *GLBasicShaders::singleTextureOpacity()
{
    if (!d->singleTextureOpacity) d->singleTextureOpacity = new SingleTextureOpacityShader();
    return d->singleTextureOpacity;
}

DualTextureAddShader *GLBasicShaders::dualTextureAdd()
{
    if (!d->dualTextureAdd) d->dualTextureAdd = new DualTextureAddShader();
    return d->dualTextureAdd;
}

SingleTextureVertexOpacityShader *GLBasicShaders::singleTextureVertexOpacity()
{
    if (!d->singleTextureVertexOpacity) d->singleTextureVertexOpacity = new SingleTextureVertexOpacityShader();
    return d->singleTextureVertexOpacity;
}

SingleTextureShadowShader *GLBasicShaders::singleTextureShadow()
{
    if (!d->singleTextureShadow) d->singleTextureShadow = new SingleTextureShadowShader();
    return d->singleTextureShadow;
}

ConstantColorShader *GLBasicShaders::constantColor()
{
    if (!d->constantColor) d->constantColor = new ConstantColorShader();
    return d->constantColor;
}

ColorShader *GLBasicShaders::color()
{
    if (!d->color) d->color = new ColorShader();
    return d->color;
}

SingleTextureOpacityShader::SingleTextureOpacityShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            attribute highp vec4 myVertex;\
            attribute mediump vec4 myUV;\
            uniform mediump mat4 myPMVMatrix;\
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex;\
                myTexCoord = myUV.st;\
            }"
            );

    frag.compile("\
            uniform sampler2D sampler2d;\
            uniform lowp float myOpacity;\
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                mediump vec4 tex = texture2D(sampler2d,myTexCoord);\
                gl_FragColor = vec4(tex.rgb, myOpacity * tex.a);\
            }"
            );

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
    bindAttributeLocation("myUV", TextureCoords);
}

bool SingleTextureOpacityShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    opacity = uniformLocation("myOpacity");
    enable();
    setUniformValue("sampler2d", 0);
    disable();
    return true;
}

void SingleTextureOpacityShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

void SingleTextureOpacityShader::setOpacity(GLfloat f)
{
    setUniformValue(opacity, f);
}

SingleTextureShadowShader::SingleTextureShadowShader()
{
    QGLShader vert(QGLShader::VertexShader);
    QGLShader frag(QGLShader::FragmentShader);

    vert.compile("\
            attribute highp vec4 myVertex;\
            attribute mediump vec4 myUV;\
            uniform mediump mat4 myPMVMatrix;\
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                gl_Position = myPMVMatrix * myVertex;\
                myTexCoord = myUV.st;\
            }"
            );

    frag.compile("\
            uniform sampler2D sampler2d;\
            uniform lowp float myOpacity;\
            varying mediump vec2 myTexCoord;\
            void main(void)\
            {\
                mediump vec4 tex = texture2D(sampler2d,myTexCoord);\
                gl_FragColor = vec4(0, 0, 0, myOpacity * tex.a * .75);\
            }"
            );

    addShader(&vert);
    addShader(&frag);

    bindAttributeLocation("myVertex", Vertices);
    bindAttributeLocation("myUV", TextureCoords);
}

bool SingleTextureShadowShader::link()
{
    if (!QGLShaderProgram::link())
        return false;
    transform = uniformLocation("myPMVMatrix");
    opacity = uniformLocation("myOpacity");
    enable();
    setUniformValue("sampler2d", 0);
    disable();
    return true;
}

void SingleTextureShadowShader::setTransform(const QMatrix4x4 &matrix)
{
    setUniformValue(transform, matrix);
}

void SingleTextureShadowShader::setOpacity(GLfloat f)
{
    setUniformValue(opacity, f);
}
QT_END_NAMESPACE
