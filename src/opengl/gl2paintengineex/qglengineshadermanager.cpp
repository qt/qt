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

#include "qglengineshadermanager_p.h"
#include "qglengineshadersource_p.h"

#if defined(QT_DEBUG)
#include <QMetaEnum>
#endif


QT_BEGIN_NAMESPACE

QGLCustomShader::QGLCustomShader(QGLShader *shader)
    : m_shader(shader)
{
}

QGLCustomShader::~QGLCustomShader()
{
    delete m_shader;
}

void QGLCustomShader::updateUniforms(QGLShaderProgram *)
{
}

void QGLCustomShader::setShader(QGLShader *shader)
{
    m_shader = shader;
}

QGLShader *QGLCustomShader::shader() const
{
    return m_shader;
}

const char* QGLEngineShaderManager::qglEngineShaderSourceCode[] = {
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0
};

QGLEngineShaderManager::QGLEngineShaderManager(QGLContext* context)
    : ctx(context),
      shaderProgNeedsChanging(true),
      srcPixelType(Qt::NoBrush),
      useGlobalOpacity(false),
      maskType(NoMask),
      useTextureCoords(false),
      compositionMode(QPainter::CompositionMode_SourceOver),
      blitShaderProg(0),
      simpleShaderProg(0),
      currentShaderProg(0),
      customShader(0)
{
    memset(compiledShaders, 0, sizeof(compiledShaders));

/*
    Rather than having the shader source array statically initialised, it is initialised
    here instead. This is to allow new shader names to be inserted or existing names moved
    around without having to change the order of the glsl strings. It is hoped this will
    make future hard-to-find runtime bugs more obvious and generally give more solid code.
*/
    static bool qglEngineShaderSourceCodePopulated = false;
    if (!qglEngineShaderSourceCodePopulated) {

        const char** code = qglEngineShaderSourceCode; // shortcut

        code[MainVertexShader] = qglslMainVertexShader;
        code[MainWithTexCoordsVertexShader] = qglslMainWithTexCoordsVertexShader;

        code[UntransformedPositionVertexShader] = qglslUntransformedPositionVertexShader;
        code[PositionOnlyVertexShader] = qglslPositionOnlyVertexShader;
        code[PositionWithPatternBrushVertexShader] = qglslPositionWithPatternBrushVertexShader;
        code[PositionWithLinearGradientBrushVertexShader] = qglslPositionWithLinearGradientBrushVertexShader;
        code[PositionWithConicalGradientBrushVertexShader] = qglslPositionWithConicalGradientBrushVertexShader;
        code[PositionWithRadialGradientBrushVertexShader] = qglslPositionWithRadialGradientBrushVertexShader;
        code[PositionWithTextureBrushVertexShader] = qglslPositionWithTextureBrushVertexShader;
        code[AffinePositionWithPatternBrushVertexShader] = qglslAffinePositionWithPatternBrushVertexShader;
        code[AffinePositionWithLinearGradientBrushVertexShader] = qglslAffinePositionWithLinearGradientBrushVertexShader;
        code[AffinePositionWithConicalGradientBrushVertexShader] = qglslAffinePositionWithConicalGradientBrushVertexShader;
        code[AffinePositionWithRadialGradientBrushVertexShader] = qglslAffinePositionWithRadialGradientBrushVertexShader;
        code[AffinePositionWithTextureBrushVertexShader] = qglslAffinePositionWithTextureBrushVertexShader;

        code[MainFragmentShader_CMO] = qglslMainFragmentShader_CMO;
        code[MainFragmentShader_CM] = qglslMainFragmentShader_CM;
        code[MainFragmentShader_MO] = qglslMainFragmentShader_MO;
        code[MainFragmentShader_M] = qglslMainFragmentShader_M;
        code[MainFragmentShader_CO] = qglslMainFragmentShader_CO;
        code[MainFragmentShader_C] = qglslMainFragmentShader_C;
        code[MainFragmentShader_O] = qglslMainFragmentShader_O;
        code[MainFragmentShader] = qglslMainFragmentShader;

        code[ImageSrcFragmentShader] = qglslImageSrcFragmentShader;
        code[CustomSrcFragmentShader] = qglslCustomSrcFragmentShader;
        code[ImageSrcWithPatternFragmentShader] = qglslImageSrcWithPatternFragmentShader;
        code[NonPremultipliedImageSrcFragmentShader] = qglslNonPremultipliedImageSrcFragmentShader;
        code[SolidBrushSrcFragmentShader] = qglslSolidBrushSrcFragmentShader;
        code[TextureBrushSrcFragmentShader] = qglslTextureBrushSrcFragmentShader;
        code[TextureBrushSrcWithPatternFragmentShader] = qglslTextureBrushSrcWithPatternFragmentShader;
        code[PatternBrushSrcFragmentShader] = qglslPatternBrushSrcFragmentShader;
        code[LinearGradientBrushSrcFragmentShader] = qglslLinearGradientBrushSrcFragmentShader;
        code[RadialGradientBrushSrcFragmentShader] = qglslRadialGradientBrushSrcFragmentShader;
        code[ConicalGradientBrushSrcFragmentShader] = qglslConicalGradientBrushSrcFragmentShader;
        code[ShockingPinkSrcFragmentShader] = qglslShockingPinkSrcFragmentShader;

        code[MaskFragmentShader] = qglslMaskFragmentShader;
        code[RgbMaskFragmentShader] = ""; //###
        code[RgbMaskWithGammaFragmentShader] = ""; //###

        code[MultiplyCompositionModeFragmentShader] = ""; //###
        code[ScreenCompositionModeFragmentShader] = ""; //###
        code[OverlayCompositionModeFragmentShader] = ""; //###
        code[DarkenCompositionModeFragmentShader] = ""; //###
        code[LightenCompositionModeFragmentShader] = ""; //###
        code[ColorDodgeCompositionModeFragmentShader] = ""; //###
        code[ColorBurnCompositionModeFragmentShader] = ""; //###
        code[HardLightCompositionModeFragmentShader] = ""; //###
        code[SoftLightCompositionModeFragmentShader] = ""; //###
        code[DifferenceCompositionModeFragmentShader] = ""; //###
        code[ExclusionCompositionModeFragmentShader] = ""; //###

#if defined(QT_DEBUG)
        // Check that all the elements have been filled:
        for (int i = 0; i < TotalShaderCount; ++i) {
            if (qglEngineShaderSourceCode[i] == 0) {
                int enumIndex = staticMetaObject.indexOfEnumerator("ShaderName");
                QMetaEnum m = staticMetaObject.enumerator(enumIndex);

                qCritical() << "qglEngineShaderSourceCode: Source for" << m.valueToKey(i)
                            << "(shader" << i << ") missing!";
            }
        }
#endif
        qglEngineShaderSourceCodePopulated = true;
    }

    // Compile up the simple shader:
    simpleShaderProg = new QGLShaderProgram(ctx, this);
    compileNamedShader(MainVertexShader,              QGLShader::PartialVertexShader);
    compileNamedShader(PositionOnlyVertexShader,      QGLShader::PartialVertexShader);
    compileNamedShader(MainFragmentShader,            QGLShader::PartialFragmentShader);
    compileNamedShader(ShockingPinkSrcFragmentShader, QGLShader::PartialFragmentShader);
    simpleShaderProg->addShader(compiledShaders[MainVertexShader]);
    simpleShaderProg->addShader(compiledShaders[PositionOnlyVertexShader]);
    simpleShaderProg->addShader(compiledShaders[MainFragmentShader]);
    simpleShaderProg->addShader(compiledShaders[ShockingPinkSrcFragmentShader]);
    simpleShaderProg->bindAttributeLocation("vertexCoordsArray", QT_VERTEX_COORDS_ATTR);
    simpleShaderProg->link();
    if (!simpleShaderProg->isLinked()) {
        qCritical() << "Errors linking simple shader:"
                    << simpleShaderProg->log();
    }

    // Compile the blit shader:
    blitShaderProg = new QGLShaderProgram(ctx, this);
    compileNamedShader(MainWithTexCoordsVertexShader,     QGLShader::PartialVertexShader);
    compileNamedShader(UntransformedPositionVertexShader, QGLShader::PartialVertexShader);
    compileNamedShader(MainFragmentShader,                QGLShader::PartialFragmentShader);
    compileNamedShader(ImageSrcFragmentShader,            QGLShader::PartialFragmentShader);
    blitShaderProg->addShader(compiledShaders[MainWithTexCoordsVertexShader]);
    blitShaderProg->addShader(compiledShaders[UntransformedPositionVertexShader]);
    blitShaderProg->addShader(compiledShaders[MainFragmentShader]);
    blitShaderProg->addShader(compiledShaders[ImageSrcFragmentShader]);
    blitShaderProg->bindAttributeLocation("textureCoordArray", QT_TEXTURE_COORDS_ATTR);
    blitShaderProg->bindAttributeLocation("vertexCoordsArray", QT_VERTEX_COORDS_ATTR);
    blitShaderProg->link();
    if (!blitShaderProg->isLinked()) {
        qCritical() << "Errors linking blit shader:"
                    << blitShaderProg->log();
    }
}

QGLEngineShaderManager::~QGLEngineShaderManager()
{
    //###
}


uint QGLEngineShaderManager::getUniformIdentifier(const char *uniformName)
{
    uniformIdentifiers << uniformName;
    return uniformIdentifiers.size() - 1;
}

uint QGLEngineShaderManager::getUniformLocation(uint id)
{
    QVector<uint> &uniformLocations = currentShaderProg->uniformLocations;
    uint oldSize = uniformLocations.size();
    if (oldSize <= id) {
        uint newSize = id + 1;
        uniformLocations.resize(newSize);

        for (uint i = oldSize; i < newSize; ++i)
            uniformLocations[i] = GLuint(-1);
    }

    if (uniformLocations.at(id) == GLuint(-1))
        uniformLocations[id] = currentShaderProg->program->uniformLocation(uniformIdentifiers.at(id));

    return uniformLocations.at(id);
}


void QGLEngineShaderManager::optimiseForBrushTransform(const QTransform &transform)
{
    Q_UNUSED(transform); // Currently ignored
}

void QGLEngineShaderManager::setDirty()
{
    shaderProgNeedsChanging = true;
}

void QGLEngineShaderManager::setSrcPixelType(Qt::BrushStyle style)
{
    if (customShader || srcPixelType == PixelSrcType(style))
        return;

    srcPixelType = style;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setSrcPixelType(PixelSrcType type)
{
    if (customShader || srcPixelType == type)
        return;

    srcPixelType = type;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setCustomShader(QGLCustomShader *shader)
{
    srcPixelType = CustomSrc;
    shaderProgNeedsChanging = true;
    customShader = shader;
}

void QGLEngineShaderManager::setTextureCoordsEnabled(bool enabled)
{
    if (useTextureCoords == enabled)
        return;

    useTextureCoords = enabled;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setUseGlobalOpacity(bool useOpacity)
{
    if (useGlobalOpacity == useOpacity)
        return;

    useGlobalOpacity = useOpacity;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setMaskType(MaskType type)
{
    if (maskType == type)
        return;

    maskType = type;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setCompositionMode(QPainter::CompositionMode mode)
{
    if (compositionMode == mode)
        return;

    compositionMode = mode;
    shaderProgNeedsChanging = true; //###
}

QGLShaderProgram* QGLEngineShaderManager::currentProgram()
{
    return currentShaderProg->program;
}

QGLShaderProgram* QGLEngineShaderManager::simpleProgram()
{
    return simpleShaderProg;
}

QGLShaderProgram* QGLEngineShaderManager::blitProgram()
{
    return blitShaderProg;
}



// Select & use the correct shader program using the current state.
// Returns true if program needed changing.
bool QGLEngineShaderManager::useCorrectShaderProg()
{
    if (!shaderProgNeedsChanging)
        return false;

    QGLEngineShaderProg requiredProgram;
    requiredProgram.program = 0;

    // Choose vertex shader main function
    QGLEngineShaderManager::ShaderName mainVertexShaderName = InvalidShaderName;
    if (useTextureCoords)
        mainVertexShaderName = MainWithTexCoordsVertexShader;
    else
        mainVertexShaderName = MainVertexShader;
    compileNamedShader(mainVertexShaderName, QGLShader::PartialVertexShader);
    requiredProgram.mainVertexShader = compiledShaders[mainVertexShaderName];

    // Choose vertex shader shader position function (which typically also sets
    // varyings) and the source pixel (srcPixel) fragment shader function:
    QGLEngineShaderManager::ShaderName positionVertexShaderName = InvalidShaderName;
    QGLEngineShaderManager::ShaderName srcPixelFragShaderName = InvalidShaderName;
    bool isAffine = brushTransform.isAffine();
    if ( (srcPixelType >= Qt::Dense1Pattern) && (srcPixelType <= Qt::DiagCrossPattern) ) {
        if (isAffine)
            positionVertexShaderName = AffinePositionWithPatternBrushVertexShader;
        else
            positionVertexShaderName = PositionWithPatternBrushVertexShader;

        srcPixelFragShaderName = PatternBrushSrcFragmentShader;
    }
    else switch (srcPixelType) {
        default:
        case Qt::NoBrush:
            qCritical("QGLEngineShaderManager::useCorrectShaderProg() - I'm scared, Qt::NoBrush style is set");
            break;
        case QGLEngineShaderManager::ImageSrc:
            srcPixelFragShaderName = ImageSrcFragmentShader;
            positionVertexShaderName = PositionOnlyVertexShader;
            break;
        case QGLEngineShaderManager::CustomSrc:
            srcPixelFragShaderName = CustomSrcFragmentShader;
            positionVertexShaderName = PositionOnlyVertexShader;
            break;
        case QGLEngineShaderManager::PatternSrc:
            srcPixelFragShaderName = ImageSrcWithPatternFragmentShader;
            positionVertexShaderName = PositionOnlyVertexShader;
            break;
        case QGLEngineShaderManager::TextureSrcWithPattern:
            srcPixelFragShaderName = TextureBrushSrcWithPatternFragmentShader;
            positionVertexShaderName = isAffine ? AffinePositionWithTextureBrushVertexShader
                                                : PositionWithTextureBrushVertexShader;
            break;
        case QGLEngineShaderManager::NonPremultipliedImageSrc:
            srcPixelFragShaderName = NonPremultipliedImageSrcFragmentShader;
            positionVertexShaderName = PositionOnlyVertexShader;
            break;
        case Qt::SolidPattern:
            srcPixelFragShaderName = SolidBrushSrcFragmentShader;
            positionVertexShaderName = PositionOnlyVertexShader;
            break;
        case Qt::LinearGradientPattern:
            srcPixelFragShaderName = LinearGradientBrushSrcFragmentShader;
            positionVertexShaderName = isAffine ? AffinePositionWithLinearGradientBrushVertexShader
                                                : PositionWithLinearGradientBrushVertexShader;
            break;
        case Qt::ConicalGradientPattern:
            srcPixelFragShaderName = ConicalGradientBrushSrcFragmentShader;
            positionVertexShaderName = isAffine ? AffinePositionWithConicalGradientBrushVertexShader
                                                : PositionWithConicalGradientBrushVertexShader;
            break;
        case Qt::RadialGradientPattern:
            srcPixelFragShaderName = RadialGradientBrushSrcFragmentShader;
            positionVertexShaderName = isAffine ? AffinePositionWithRadialGradientBrushVertexShader
                                                : PositionWithRadialGradientBrushVertexShader;
            break;
        case Qt::TexturePattern:
            srcPixelFragShaderName = TextureBrushSrcFragmentShader;
            positionVertexShaderName = isAffine ? AffinePositionWithTextureBrushVertexShader
                                                : PositionWithTextureBrushVertexShader;
            break;
    };
    compileNamedShader(positionVertexShaderName, QGLShader::PartialVertexShader);
    compileNamedShader(srcPixelFragShaderName, QGLShader::PartialFragmentShader);
    requiredProgram.positionVertexShader = compiledShaders[positionVertexShaderName];
    requiredProgram.srcPixelFragShader = compiledShaders[srcPixelFragShaderName];

    const bool hasCompose = compositionMode > QPainter::CompositionMode_Plus;
    const bool hasMask = maskType != QGLEngineShaderManager::NoMask;

    // Choose fragment shader main function:
    QGLEngineShaderManager::ShaderName mainFragShaderName;

    if (hasCompose && hasMask && useGlobalOpacity)
        mainFragShaderName = MainFragmentShader_CMO;
    if (hasCompose && hasMask && !useGlobalOpacity)
        mainFragShaderName = MainFragmentShader_CM;
    if (!hasCompose && hasMask && useGlobalOpacity)
        mainFragShaderName = MainFragmentShader_MO;
    if (!hasCompose && hasMask && !useGlobalOpacity)
        mainFragShaderName = MainFragmentShader_M;
    if (hasCompose && !hasMask && useGlobalOpacity)
        mainFragShaderName = MainFragmentShader_CO;
    if (hasCompose && !hasMask && !useGlobalOpacity)
        mainFragShaderName = MainFragmentShader_C;
    if (!hasCompose && !hasMask && useGlobalOpacity)
        mainFragShaderName = MainFragmentShader_O;
    if (!hasCompose && !hasMask && !useGlobalOpacity)
        mainFragShaderName = MainFragmentShader;

    compileNamedShader(mainFragShaderName, QGLShader::PartialFragmentShader);
    requiredProgram.mainFragShader = compiledShaders[mainFragShaderName];

    if (hasMask) {
        QGLEngineShaderManager::ShaderName maskShaderName = QGLEngineShaderManager::InvalidShaderName;
        if (maskType == PixelMask)
            maskShaderName = MaskFragmentShader;
        else if (maskType == SubPixelMask)
            maskShaderName = RgbMaskFragmentShader;
        else if (maskType == SubPixelWithGammaMask)
            maskShaderName = RgbMaskWithGammaFragmentShader;
        else
            qCritical("QGLEngineShaderManager::useCorrectShaderProg() - Unknown mask type");

        compileNamedShader(maskShaderName, QGLShader::PartialFragmentShader);
        requiredProgram.maskFragShader = compiledShaders[maskShaderName];
    }
    else
        requiredProgram.maskFragShader = 0;

    if (hasCompose) {
        QGLEngineShaderManager::ShaderName compositionShaderName = QGLEngineShaderManager::InvalidShaderName;
        switch (compositionMode) {
            case QPainter::CompositionMode_Multiply:
                compositionShaderName = MultiplyCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Screen:
                compositionShaderName = ScreenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Overlay:
                compositionShaderName = OverlayCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Darken:
                compositionShaderName = DarkenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Lighten:
                compositionShaderName = LightenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_ColorDodge:
                compositionShaderName = ColorDodgeCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_ColorBurn:
                compositionShaderName = ColorBurnCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_HardLight:
                compositionShaderName = HardLightCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_SoftLight:
                compositionShaderName = SoftLightCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Difference:
                compositionShaderName = DifferenceCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Exclusion:
                compositionShaderName = ExclusionCompositionModeFragmentShader;
                break;
            default:
                qWarning("QGLEngineShaderManager::useCorrectShaderProg() - Unsupported composition mode");
        }
        compileNamedShader(compositionShaderName, QGLShader::PartialFragmentShader);
        requiredProgram.compositionFragShader = compiledShaders[compositionShaderName];
    }
    else
        requiredProgram.compositionFragShader = 0;


    // At this point, requiredProgram is fully populated so try to find the program in the cache
    for (int i = 0; i < cachedPrograms.size(); ++i) {
        QGLEngineShaderProg &prog = cachedPrograms[i];
        if (    (prog.mainVertexShader == requiredProgram.mainVertexShader)
             && (prog.positionVertexShader == requiredProgram.positionVertexShader)
             && (prog.mainFragShader == requiredProgram.mainFragShader)
             && (prog.srcPixelFragShader == requiredProgram.srcPixelFragShader)
             && (prog.compositionFragShader == requiredProgram.compositionFragShader) )
        {
            currentShaderProg = &prog;
            currentShaderProg->program->enable();
            shaderProgNeedsChanging = false;

            return true;
        }
    }

    // Shader program not found in cache, create it now.
    requiredProgram.program = new QGLShaderProgram(ctx, this);
    requiredProgram.program->addShader(requiredProgram.mainVertexShader);
    requiredProgram.program->addShader(requiredProgram.positionVertexShader);
    requiredProgram.program->addShader(requiredProgram.mainFragShader);
    requiredProgram.program->addShader(requiredProgram.srcPixelFragShader);
    requiredProgram.program->addShader(requiredProgram.maskFragShader);
    requiredProgram.program->addShader(requiredProgram.compositionFragShader);

    if (customShader)
        requiredProgram.program->addShader(customShader->shader());

    // We have to bind the vertex attribute names before the program is linked:
    requiredProgram.program->bindAttributeLocation("vertexCoordsArray", QT_VERTEX_COORDS_ATTR);
    if (useTextureCoords)
        requiredProgram.program->bindAttributeLocation("textureCoordArray", QT_TEXTURE_COORDS_ATTR);

    requiredProgram.program->link();
    if (!requiredProgram.program->isLinked()) {
        QString error;
        qWarning() <<  "Shader program failed to link,"
#if defined(QT_DEBUG)
              << '\n'
              << "  Shaders Used:" << '\n'
              << "    mainVertexShader = "     << requiredProgram.mainVertexShader->objectName()  << '\n'
              << "    positionVertexShader = " << requiredProgram.positionVertexShader->objectName() << '\n'
              << "    mainFragShader = "       << requiredProgram.mainFragShader->objectName() << '\n'
              << "    srcPixelFragShader = "   << requiredProgram.srcPixelFragShader->objectName() << '\n'
              << "    maskFragShader = "       << requiredProgram.maskFragShader->objectName() << '\n'
              << "    compositionFragShader = "<< requiredProgram.compositionFragShader->objectName() << '\n'
#endif
              << "  Error Log:" << '\n'
              << "    " << requiredProgram.program->log();
        qWarning() << error;
    }
    else {
        if (customShader) {
            // don't cache custom shaders
            customShaderProg = requiredProgram;
            currentShaderProg = &customShaderProg;
        } else {
            cachedPrograms.append(requiredProgram);
            // taking the address here is safe since
            // cachePrograms isn't resized anywhere else
            currentShaderProg = &cachedPrograms.last();
        }
        currentShaderProg->program->enable();

        if (customShader)
            customShader->updateUniforms(currentShaderProg->program);
    }
    shaderProgNeedsChanging = false;
    return true;
}

void QGLEngineShaderManager::compileNamedShader(QGLEngineShaderManager::ShaderName name, QGLShader::ShaderType type)
{
    if (compiledShaders[name])
        return;

    QGLShader *newShader = new QGLShader(type, ctx, this);
    newShader->compile(qglEngineShaderSourceCode[name]);

#if defined(QT_DEBUG)
    // Name the shader for easier debugging
    QMetaEnum m = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("ShaderName"));
    newShader->setObjectName(QLatin1String(m.valueToKey(name)));
#endif

    compiledShaders[name] = newShader;
}

QT_END_NAMESPACE
