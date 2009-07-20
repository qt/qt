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
    GLSL function calls with the following signatures:

    Brushes & image drawing are implementations of "qcolorp vec4 srcPixel()":
        qglslImageSrcFragShader
        qglslImageSrcWithPatternFragShader
        qglslNonPremultipliedImageSrcFragShader
        qglslSolidBrushSrcFragShader
        qglslTextureBrushSrcFragShader
        qglslTextureBrushWithPatternFragShader
        qglslPatternBrushSrcFragShader
        qglslLinearGradientBrushSrcFragShader
        qglslRadialGradientBrushSrcFragShader
        qglslConicalGradientBrushSrcFragShader
    NOTE: It is assumed the colour returned by srcPixel() is pre-multiplied

    Masks are implementations of "qcolorp vec4 applyMask(qcolorp vec4 src)":
        qglslMaskFragmentShader
        qglslRgbMaskFragmentShader
        qglslRgbMaskWithGammaFragmentShader

    Composition modes are "qcolorp vec4 compose(qcolorp vec4 src)":
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


    Note: In the future, some GLSL compilers will support an extension allowing
          a new 'color' precision specifier. To support this, qcolorp is used for
          all color components so it can be defined to colorp or lowp depending upon
          the implementation.

    So there are differnt frament shader main functions, depending on the
    number & type of pipelines the fragment needs to go through.

    The choice of which main() fragment shader string to use depends on:
        - Use of global opacity
        - Brush style (some brushes apply opacity themselves)
        - Use & type of mask (TODO: Need to support high quality anti-aliasing & text)
        - Use of non-GL Composition mode

    Leading to the following fragment shader main functions:
        gl_FragColor = compose(applyMask(srcPixel()*globalOpacity));
        gl_FragColor = compose(applyMask(srcPixel()));
        gl_FragColor = applyMask(srcPixel()*globalOpacity);
        gl_FragColor = applyMask(srcPixel());
        gl_FragColor = compose(srcPixel()*globalOpacity);
        gl_FragColor = compose(srcPixel());
        gl_FragColor = srcPixel()*globalOpacity;
        gl_FragColor = srcPixel();

    Called:
        qglslMainFragmentShader_CMO
        qglslMainFragmentShader_CM
        qglslMainFragmentShader_MO
        qglslMainFragmentShader_M
        qglslMainFragmentShader_CO
        qglslMainFragmentShader_C
        qglslMainFragmentShader_O
        qglslMainFragmentShader

    Where:
        M = Mask
        C = Composition
        O = Global Opacity


    CUSTOM SHADER CODE (idea, depricated)
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

#ifndef QGLENGINE_SHADER_MANAGER_H
#define QGLENGINE_SHADER_MANAGER_H

#include <QGLShader>
#include <QGLShaderProgram>
#include <QPainter>
#include <private/qgl_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(OpenGL)


struct QGLEngineShaderProg
{
    QGLShader*          mainVertexShader;
    QGLShader*          positionVertexShader;
    QGLShader*          mainFragShader;
    QGLShader*          srcPixelFragShader;
    QGLShader*          maskFragShader;        // Can be null for no mask
    QGLShader*          compositionFragShader; // Can be null for GL-handled mode
    QGLShaderProgram*   program;

    QVector<uint> uniformLocations;
};

/*
struct QGLEngineCachedShaderProg
{
    QGLEngineCachedShaderProg(QGLEngineShaderManager::ShaderName vertexMain,
                              QGLEngineShaderManager::ShaderName vertexPosition,
                              QGLEngineShaderManager::ShaderName fragMain,
                              QGLEngineShaderManager::ShaderName pixelSrc,
                              QGLEngineShaderManager::ShaderName mask,
                              QGLEngineShaderManager::ShaderName composition);

    int cacheKey;
    QGLShaderProgram* program;
}
*/

static const GLuint QT_VERTEX_COORDS_ATTR  = 0;
static const GLuint QT_TEXTURE_COORDS_ATTR = 1;

class QGLEngineShaderManager : public QObject
{
    Q_OBJECT
public:
    QGLEngineShaderManager(QGLContext* context);
    ~QGLEngineShaderManager();

    enum MaskType {NoMask, PixelMask, SubPixelMask, SubPixelWithGammaMask};
    enum PixelSrcType {
        ImageSrc = Qt::TexturePattern+1,
        NonPremultipliedImageSrc = Qt::TexturePattern+2,
        PatternSrc = Qt::TexturePattern+3,
        TextureSrcWithPattern = Qt::TexturePattern+4
    };

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

    // There are optimisations we can do, depending on the brush transform:
    //    1) May not have to apply perspective-correction
    //    2) Can use lower precision for matrix
    void optimiseForBrushTransform(const QTransform &transform);
    void setSrcPixelType(Qt::BrushStyle);
    void setSrcPixelType(PixelSrcType); // For non-brush sources, like pixmaps & images
    void setTextureCoordsEnabled(bool); // For images & text glyphs
    void setUseGlobalOpacity(bool);
    void setMaskType(MaskType);
    void setCompositionMode(QPainter::CompositionMode);

    uint getUniformLocation(Uniform id);

    void setDirty(); // someone has manually changed the current shader program
    bool useCorrectShaderProg(); // returns true if the shader program needed to be changed

    QGLShaderProgram* currentProgram(); // Returns pointer to the shader the manager has chosen
    QGLShaderProgram* simpleProgram(); // Used to draw into e.g. stencil buffers
    QGLShaderProgram* blitProgram(); // Used to blit a texture into the framebuffer

    static QGLEngineShaderManager *managerForContext(const QGLContext *context);

    enum ShaderName {
        MainVertexShader,
        MainWithTexCoordsVertexShader,

        UntransformedPositionVertexShader,
        PositionOnlyVertexShader,
        PositionWithPatternBrushVertexShader,
        PositionWithLinearGradientBrushVertexShader,
        PositionWithConicalGradientBrushVertexShader,
        PositionWithRadialGradientBrushVertexShader,
        PositionWithTextureBrushVertexShader,
        AffinePositionWithPatternBrushVertexShader,
        AffinePositionWithLinearGradientBrushVertexShader,
        AffinePositionWithConicalGradientBrushVertexShader,
        AffinePositionWithRadialGradientBrushVertexShader,
        AffinePositionWithTextureBrushVertexShader,

        MainFragmentShader_CMO,
        MainFragmentShader_CM,
        MainFragmentShader_MO,
        MainFragmentShader_M,
        MainFragmentShader_CO,
        MainFragmentShader_C,
        MainFragmentShader_O,
        MainFragmentShader,

        ImageSrcFragmentShader,
        ImageSrcWithPatternFragmentShader,
        NonPremultipliedImageSrcFragmentShader,
        SolidBrushSrcFragmentShader,
        TextureBrushSrcFragmentShader,
        TextureBrushSrcWithPatternFragmentShader,
        PatternBrushSrcFragmentShader,
        LinearGradientBrushSrcFragmentShader,
        RadialGradientBrushSrcFragmentShader,
        ConicalGradientBrushSrcFragmentShader,
        ShockingPinkSrcFragmentShader,

        MaskFragmentShader,
        RgbMaskFragmentShader,
        RgbMaskWithGammaFragmentShader,

        MultiplyCompositionModeFragmentShader,
        ScreenCompositionModeFragmentShader,
        OverlayCompositionModeFragmentShader,
        DarkenCompositionModeFragmentShader,
        LightenCompositionModeFragmentShader,
        ColorDodgeCompositionModeFragmentShader,
        ColorBurnCompositionModeFragmentShader,
        HardLightCompositionModeFragmentShader,
        SoftLightCompositionModeFragmentShader,
        DifferenceCompositionModeFragmentShader,
        ExclusionCompositionModeFragmentShader,

        TotalShaderCount, InvalidShaderName
    };


/*
    // These allow the ShaderName enum to be used as a cache key
    const int mainVertexOffset = 0;
    const int positionVertexOffset = (1<<2) - PositionOnlyVertexShader;
    const int mainFragOffset = (1<<6) - MainFragmentShader_CMO;
    const int srcPixelOffset = (1<<10) - ImageSrcFragmentShader;
    const int maskOffset = (1<<14) - NoMaskShader;
    const int compositionOffset = (1 << 16) - MultiplyCompositionModeFragmentShader;
*/

#if defined (QT_DEBUG)
    Q_ENUMS(ShaderName)
#endif


private:
    QGLContext*     ctx;
    bool            shaderProgNeedsChanging;

    // Current state variables which influence the choice of shader:
    QTransform                  brushTransform;
    int                         srcPixelType;
    bool                        useGlobalOpacity;
    MaskType                    maskType;
    bool                        useTextureCoords;
    QPainter::CompositionMode   compositionMode;

    QGLShaderProgram*     blitShaderProg;
    QGLShaderProgram*     simpleShaderProg;
    QGLEngineShaderProg*  currentShaderProg;

    // TODO: Possibly convert to a LUT
    QList<QGLEngineShaderProg> cachedPrograms;

    QGLShader* compiledShaders[TotalShaderCount];

    void compileNamedShader(QGLEngineShaderManager::ShaderName name, QGLShader::ShaderType type);

    static const char* qglEngineShaderSourceCode[TotalShaderCount];
};

QT_END_NAMESPACE

QT_END_HEADER

#endif //QGLENGINE_SHADER_MANAGER_H
