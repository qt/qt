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

#ifndef GLGC_SHADER_SOURCE_H
#define GLGC_SHADER_SOURCE_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(OpenGL)


/*
    VERTEX SHADERS
    ==============

    Vertex shaders are specified as multiple (partial) shaders. On desktop,
    this works fine. On ES, QGLShader & QGLShaderProgram will make partial
    shaders work by concatenating the source in each QGLShader and compiling
    it as a single shader. This is abstracted nicely by QGLShaderProgram and
    the GL2 engine doesn't need to worry about it.

    Generally, there's two vertex shader objects. The position shaders are
    the ones which set gl_Position. There's also two "main" vertex shaders,
    one which just calls the position shader and another which also passes
    through some texture coordinates from a vertex attribute array to a
    varying. These texture coordinates are used for mask position in text
    rendering and for the source coordinates in drawImage/drawPixmap. There's
    also a "Simple" vertex shader for rendering a solid colour (used to render
    into the stencil buffer where the actual colour value is discarded).

    The position shaders for brushes look scary. This is because many of the
    calculations which logically belong in the fragment shader have been moved
    into the vertex shader to improve performance. This is why the position
    calculation is in a seperate shader. Not only does it calculate the
    position, but it also calculates some data to be passed to the fragment
    shader as a varying. It is optimal to move as much of the calculation as
    possible into the vertex shader as this is executed less often.

    The varyings passed to the fragment shaders are interpolated (which is
    cheap). Unfortunately, GL will apply perspective correction to the
    interpolation calusing errors. To get around this, the vertex shader must
    apply perspective correction itself and set the w-value of gl_Position to
    zero. That way, GL will be tricked into thinking it doesn't need to apply a
    perspective correction and use linear interpolation instead (which is what
    we want). Of course, if the brush transform is affeine, no perspective
    correction is needed and a simpler vertex shader can be used instead.

    So there are the following "main" vertex shaders:
        qglslSimpleVertexShader
        qglslMainVertexShader
        qglslMainWithTexCoordsVertexShader

    And the the following position vertex shaders:
        qglslPositionOnlyVertexShader
        qglslPositionWithTextureBrushVertexShader
        qglslPositionWithPatternBrushVertexShader
        qglslPositionWithLinearGradientBrushVertexShader
        qglslPositionWithRadialGradientBrushVertexShader
        qglslPositionWithConicalGradientBrushVertexShader
        qglslAffinePositionWithTextureBrushVertexShader
        qglslAffinePositionWithPatternBrushVertexShader
        qglslAffinePositionWithLinearGradientBrushVertexShader
        qglslAffinePositionWithRadialGradientBrushVertexShader
        qglslAffinePositionWithConicalGradientBrushVertexShader

    Leading to 23 possible vertex shaders


    FRAGMENT SHADERS
    ================

    Fragment shaders are also specified as multiple (partial) shaders. The
    different fragment shaders represent the different stages in Qt's fragment
    pipeline. There are 1-3 stages in this pipeline: First stage is to get the
    fragment's colour value. The next stage is to get the fragment's mask value
    (coverage value for anti-aliasing) and the final stage is to blend the
    incoming fragment with the background (for composition modes not supported
    by GL).

    Of these, the first stage will always be present. If Qt doesn't need to
    apply anti-aliasing (because it's off or handled by multisampling) then
    the coverage value doesn't need to be applied. (Note: There are two types
    of mask, one for regular anti-aliasing and one for sub-pixel anti-
    aliasing.) If the composition mode is one which GL supports natively then
    the blending stage doesn't need to be applied.

    As eash stage can have multiple implementations, they are abstracted as
    GLSL function calls, with the following signatures:

    Brushes & image drawing are implementations of "mediump vec4 srcPixel()":
        qglslImageFragmentShader
        qglslNonPremultipliedImageFragmentShader
        qglslSolidBrushFragmentShader
        qglslTextureBrushFragmentShader
        qglslPatternBrushFragmentShader
        qglslLinearGradientBrushFragmentShader
        qglslRadialGradientBrushFragmentShader
        qglslConicalGradientBrushFragmentShader

    NOTE: It is assumed the colour returned by srcPixel() is pre-multiplied

    The two masks are have these signature and are called:
        qglslMaskFragmentShader: "mediump float mask()"
        qglslRgbMaskFragmentShader: "mediump vec3 rgbMask()"

    Composition modes are "mediump vec4 composite(mediump vec4 src, mediump vec4 dest)":
        qglslColorBurnCompositionModeFragmentShader
        qglslColorDodgeCompositionModeFragmentShader
        qglslDarkenCompositionModeFragmentShader
        qglslDifferenceCompositionModeFragmentShader
        qglslExclusionCompositionModeFragmentShader
        qglslHardLightCompositionModeFragmentShader
        qglslLightenCompositionModeFragmentShader
        qglslMultiplyCompositionModeFragmentShader
        qglslOverlayCompositionModeFragmentShader
        qglslScreenCompositionModeFragmentShader
        qglslSoftLightCompositionModeFragmentShader


    So there are differnt frament shader main functions, depending on the
    number & type of pipelines the fragment needs to go through.

    The choice of which main() fragment shader string to use depends on:
        - Use of global opacity
        - Brush style (some brushes apply opacity themselves)
        - Use & type of mask (TODO: Need to support high quality anti-aliasing & text)
        - Use of gamma-correction
        - Use of non-GL Composition mode


    CUSTOM SHADER CODE (idea)
    ==================

    The use of custom shader code is supported by the engine for drawImage and
    drawPixmap calls. This is implemented via hooks in the fragment pipeline.
    The custom shader is passed to the engine as a partial fragment shader
    (QGLCustomizedShader). The shader will implement a pre-defined method name
    which Qt's fragment pipeline will call. There are two different hooks which
    can be implemented as custom shader code:

        mediump vec4 customShader(sampler2d src, vec2 srcCoords)
        mediump vec4 customShaderWithDest(sampler2d dest, sampler2d src, vec2 srcCoords)

*/

/////////////////////////////////////////////////////////////////////


static const char* qglslImageVertexShader = "\
    attribute highp vec4    inputVertex; \
    attribute lowp  vec2    textureCoord; \
    uniform   highp mat4    pmvMatrix; \
    varying   lowp  vec2    fragTextureCoord; \
    void main(void) \
    {\
            gl_Position = pmvMatrix * inputVertex;\
            fragTextureCoord = textureCoord; \
    }";

static const char* qglslImageFragmentShader = "\
    varying lowp vec2      fragTextureCoord;\
    uniform      sampler2D textureSampler;\
    uniform lowp float     opacity; \
    void main(void) \
    {\
        gl_FragColor = texture2D(textureSampler, fragTextureCoord) * opacity; \
    }";

static const char* qglslTextFragmentShader = "\
    varying lowp    vec2      fragTextureCoord;\
    uniform mediump vec4      fragmentColor;\
    uniform         sampler2D textureSampler;\
    void main(void) \
    {\
        highp vec4 tex = texture2D(textureSampler, fragTextureCoord); \
        tex = fragmentColor * tex.r; \
        gl_FragColor = tex; \
    }";

static const char* qglslDefaultVertexShader = "\
    attribute highp vec4    inputVertex;\
    uniform   highp mat4    pmvMatrix;\
    void main(void)\
    {\
            gl_Position = pmvMatrix * inputVertex;\
    }";

static const char* qglslSimpleFragmentShader = "\
    void main (void)\
    {\
            gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\
    }";


/**** FRAGMENT SHADER MAIN FUNCTIONS ****/
// NOTE: Currently, the engine assumes brushes return colors already in pre-multiplied
//       format. However, this may change if we add support for non-premultiplied 

static const char* qglslNoOpacityFragmentShaderMain = "\n\
    mediump vec4 brush();\
    void main (void)\
    {\
            gl_FragColor = brush();\
    }\n";

static const char* qglslFragmentShaderMain = "\n\
    mediump vec4    brush();\
    uniform lowp    float   opacity; \
    void main (void)\
    {\
        gl_FragColor = brush() * opacity;\
    }\n";



/**** BRUSH SHADERS ****/

// This should never actually be used
static const char* qglslNoBrushFragmentShader = "\n\
    mediump vec4 brush() { \
        discard; \
        return vec4(1.0, 0.8, 0.8, 1.0);\
    }\n";

// Solid Fill Brush
static const char* qglslSolidBrushFragmentShader = "\n\
    uniform mediump vec4 fragmentColor; \
    mediump vec4 brush() { \
        return fragmentColor;\
    }\n";

// Texture Brush
static const char* qglslTextureBrushVertexShader = "\
    attribute highp   vec4  inputVertex; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   mediump vec2  invertedTextureSize; \
    uniform   mediump mat3  brushTransform; \
    varying   mediump vec2  texCoords; \
    void main(void) { \
            gl_Position = pmvMatrix * inputVertex;\
            gl_Position.xy = gl_Position.xy / gl_Position.w; \
            mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
            mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
            mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
            gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
            gl_Position.w = invertedHTexCoordsZ; \
            texCoords.xy = (hTexCoords.xy * invertedTextureSize) * gl_Position.w; \
            texCoords.y = -texCoords.y; \
    }";

static const char* qglslTextureBrushFragmentShader = "\n\
    varying mediump vec2      texCoords;\
    uniform         sampler2D brushTexture;\
    mediump vec4 brush() { \
        return texture2D(brushTexture, texCoords); \
    }\n";


// Pattern Brush - This assumes the texture size is 8x8 and thus, the inverted size is 0.125
static const char* qglslPatternBrushVertexShader = "\
    attribute highp   vec4  inputVertex; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   mediump vec2  invertedTextureSize; \
    uniform   mediump mat3  brushTransform; \
    varying   mediump vec2  texCoords; \
    void main(void) { \
            gl_Position = pmvMatrix * inputVertex;\
            gl_Position.xy = gl_Position.xy / gl_Position.w; \
            mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
            mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
            mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
            gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
            gl_Position.w = invertedHTexCoordsZ; \
            texCoords.xy = (hTexCoords.xy * 0.125) * invertedHTexCoordsZ; \
            texCoords.y = -texCoords.y; \
    }";

static const char* qglslPatternBrushFragmentShader = "\n\
    uniform         sampler2D brushTexture;\
    uniform lowp    vec4      patternColor; \
    varying mediump vec2      texCoords;\
    mediump vec4 brush() { \
        return patternColor * texture2D(brushTexture, texCoords).r; \
    }\n";


// Linear Gradient Brush
static const char* qglslLinearGradientBrushVertexShader = "\
    attribute highp   vec4  inputVertex; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   highp   vec3  linearData; \
    uniform   mediump mat3  brushTransform; \
    varying   mediump float index ; \
    void main() { \
        gl_Position = pmvMatrix * inputVertex;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        index = (dot(linearData.xy, hTexCoords.xy) * linearData.z) * invertedHTexCoordsZ; \
    }";

static const char* qglslLinearGradientBrushFragmentShader = "\n\
    uniform         sampler2D   brushTexture; \
    varying mediump float       index; \
    mediump vec4 brush() { \
        mediump vec2 val = vec2(index, 0.5); \
        return texture2D(brushTexture, val); \
    }\n";


static const char* qglslRadialGradientBrushVertexShader = "\
    attribute highp   vec4 inputVertex;\
    uniform   highp   mat4 pmvMatrix;\
    uniform   mediump vec2 halfViewportSize; \
    uniform   highp   mat3 brushTransform; \
    uniform   highp   vec2 fmp; \
    varying   highp   float b; \
    varying   highp   vec2  A; \
    void main(void) \
    {\
        gl_Position = pmvMatrix * inputVertex;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        A = hTexCoords.xy * invertedHTexCoordsZ; \
        b = 2.0 * fmp * (A.x + A.y); \
\
    }";

static const char* qglslRadialGradientBrushFragmentShader = "\n\
    uniform         sampler2D   brushTexture; \
    uniform highp   float       fmp2_m_radius2; \
    uniform highp   float       inverse_2_fmp2_m_radius2; \
    varying   highp   float b; \
    varying   highp   vec2  A; \
\
    mediump vec4 brush() { \
        highp float c = -dot(A, A); \
        highp vec2 val = vec2((-b + sqrt(b*b - 4.0*fmp2_m_radius2*c)) * inverse_2_fmp2_m_radius2, 0.5); \
        return texture2D(brushTexture, val); \
    }\n";

static const char* qglslConicalGradientBrushVertexShader = "\
    attribute highp vec4    inputVertex;\
    uniform   highp mat4    pmvMatrix;\
    uniform   mediump vec2  halfViewportSize; \
    uniform highp   mat3    brushTransform; \
    varying highp   vec2    A; \
    void main(void)\
    {\
        gl_Position = pmvMatrix * inputVertex;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2  viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        A = hTexCoords.xy * invertedHTexCoordsZ; \
    }";

static const char* qglslConicalGradientBrushFragmentShader = "\n\
    #define INVERSE_2PI 0.1591549430918953358 \n\
    uniform         sampler2D   brushTexture; \
    uniform mediump float       angle; \
    varying highp   vec2        A; \
    mediump vec4 brush() { \
        if (abs(A.y) == abs(A.x)) \
            A.y += 0.002; \
        highp float t = (atan2(-A.y, A.x) + angle) * INVERSE_2PI; \
        return texture2D(brushTexture, vec2(t - floor(t), 0.5)); \
    }\n";


QT_END_NAMESPACE

QT_END_HEADER

#endif // GLGC_SHADER_SOURCE_H
