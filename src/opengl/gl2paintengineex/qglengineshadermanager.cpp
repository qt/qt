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

#include "qglengineshadermanager_p.h"

#if defined(QT_DEBUG)
#include <QMetaEnum>
#endif


QGLEngineShaderManager::QGLEngineShaderManager(QGLContext* context)
    : ctx(context),
      shaderProgNeedsChanging(true),
      srcPixelType(Qt::NoBrush),
      useGlobalOpacity(false),
      maskType(NoMask),
      useTextureCoords(false),
      compositionMode(QPainter::CompositionMode_SourceOver),
      simpleShaderProg(0),
      currentShaderProg(0)
{
    memset(compiledShaders, 0, sizeof(compiledShaders));
}

QGLEngineShaderManager::~QGLEngineShaderManager()
{
    //###
}





void QGLEngineShaderManager::optimiseForBrushTransform(const QTransform transform)
{
    Q_UNUSED(transform); // Currently ignored
}

void QGLEngineShaderManager::setSrcPixelType(Qt::BrushStyle style)
{
    srcPixelType = style;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setSrcPixelType(PixelSrcType type)
{
    srcPixelType = type;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setTextureCoordsEnabled(bool enabled)
{
    useTextureCoords = enabled;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setUseGlobalOpacity(bool useOpacity)
{
    useGlobalOpacity = useOpacity;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setMaskType(MaskType type)
{
    maskType = type;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setCompositionMode(QPainter::CompositionMode mode)
{
    compositionMode = mode;
    shaderProgNeedsChanging = true; //###
}

bool QGLEngineShaderManager::shaderProgramDirty()
{
    return shaderProgNeedsChanging;
}

QGLShaderProgram* QGLEngineShaderManager::currentProgram()
{
    return currentShaderProg;
}

QGLShaderProgram* QGLEngineShaderManager::simpleProgram()
{
    return simpleShaderProg;
}




// Select & use the correct shader program using the current state
void QGLEngineShaderManager::useCorrectShaderProg()
{
    QGLEngineShaderProg requiredProgram;

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
    bool isAffine = transform.isAffine();
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
    foreach (const QGLEngineShaderProg &prog, cachedPrograms) {
        if (    (prog.mainVertexShader == requiredProgram.mainVertexShader)
             && (prog.positionVertexShader == requiredProgram.positionVertexShader)
             && (prog.mainFragShader == requiredProgram.mainFragShader)
             && (prog.srcPixelFragShader == requiredProgram.srcPixelFragShader)
             && (prog.compositionFragShader == requiredProgram.compositionFragShader) )
        {
            currentShaderProg = requiredProgram.program;
            currentShaderProg->enable();
            shaderProgNeedsChanging = false;
            return;
        }
    }

/*
    QGLShader*          mainVertexShader;
    QGLShader*          positionVertexShader;
    QGLShader*          mainFragShader;
    QGLShader*          srcPixelFragShader;
    QGLShader*          maskFragShader;        // Can be null for no mask
    QGLShader*          compositionFragShader; // Can be null for GL-handled mode
    QGLShaderProgram*   program;
*/
    // Shader program not found in cache, create it now.
    requiredProgram.program = new QGLShaderProgram(ctx, this);
    requiredProgram.program->addShader(requiredProgram.mainVertexShader);
    requiredProgram.program->addShader(requiredProgram.positionVertexShader);
    requiredProgram.program->addShader(requiredProgram.mainFragShader);
    requiredProgram.program->addShader(requiredProgram.srcPixelFragShader);
    requiredProgram.program->addShader(requiredProgram.maskFragShader);
    requiredProgram.program->addShader(requiredProgram.compositionFragShader);
    requiredProgram.program->link();
    if (!requiredProgram.program->isValid()) {
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
              << "    " << requiredProgram.program->errors();
        qWarning() << error;
    }
    else {
        cachedPrograms.append(requiredProgram);
        currentShaderProg = requiredProgram.program;
        currentShaderProg->enable();
    }
    shaderProgNeedsChanging = false;
}

void QGLEngineShaderManager::compileNamedShader(QGLEngineShaderManager::ShaderName name, QGLShader::ShaderType type)
{
    if (compiledShaders[name])
        return;

    QGLShader *newShader = new QGLShader(type, ctx, this);
    newShader->setSourceCode(qglEngineShaderSourceCode[name]);
    // newShader->compile(); ### does not exist?

#if defined(QT_DEBUG)
    // Name the shader for easier debugging
    QMetaEnum m = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("ShaderName"));
    newShader->setObjectName(QLatin1String(m.valueToKey(name)));
#endif

    compiledShaders[name] = newShader;
}


