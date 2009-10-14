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

#include "qglengineshadermanager_p.h"
#include "qglengineshadersource_p.h"

#if defined(QT_DEBUG)
#include <QMetaEnum>
#endif


QT_BEGIN_NAMESPACE

static void qt_shared_shaders_free(void *data)
{
    delete reinterpret_cast<QGLEngineSharedShaders *>(data);
}

Q_GLOBAL_STATIC_WITH_ARGS(QGLContextResource, qt_shared_shaders, (qt_shared_shaders_free))

QGLEngineSharedShaders *QGLEngineSharedShaders::shadersForContext(const QGLContext *context)
{
    QGLEngineSharedShaders *p = reinterpret_cast<QGLEngineSharedShaders *>(qt_shared_shaders()->value(context));
    if (!p) {
        QGLShareContextScope scope(context);
        qt_shared_shaders()->insert(context, p = new QGLEngineSharedShaders(context));
    }
    return p;
}

const char* QGLEngineSharedShaders::qglEngineShaderSourceCode[] = {
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0
};

QGLEngineSharedShaders::QGLEngineSharedShaders(const QGLContext* context)
    : ctxGuard(context)
    , blitShaderProg(0)
    , simpleShaderProg(0)
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
        code[MainWithTexCoordsAndOpacityVertexShader] = qglslMainWithTexCoordsAndOpacityVertexShader;

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
        code[MainFragmentShader_ImageArrays] = qglslMainFragmentShader_ImageArrays;

        code[ImageSrcFragmentShader] = qglslImageSrcFragmentShader;
        code[ImageSrcWithPatternFragmentShader] = qglslImageSrcWithPatternFragmentShader;
        code[NonPremultipliedImageSrcFragmentShader] = qglslNonPremultipliedImageSrcFragmentShader;
        code[CustomImageSrcFragmentShader] = ""; // Supplied by app.
        code[SolidBrushSrcFragmentShader] = qglslSolidBrushSrcFragmentShader;
        code[TextureBrushSrcFragmentShader] = qglslTextureBrushSrcFragmentShader;
        code[TextureBrushSrcWithPatternFragmentShader] = qglslTextureBrushSrcWithPatternFragmentShader;
        code[PatternBrushSrcFragmentShader] = qglslPatternBrushSrcFragmentShader;
        code[LinearGradientBrushSrcFragmentShader] = qglslLinearGradientBrushSrcFragmentShader;
        code[RadialGradientBrushSrcFragmentShader] = qglslRadialGradientBrushSrcFragmentShader;
        code[ConicalGradientBrushSrcFragmentShader] = qglslConicalGradientBrushSrcFragmentShader;
        code[ShockingPinkSrcFragmentShader] = qglslShockingPinkSrcFragmentShader;

        code[MaskFragmentShader] = qglslMaskFragmentShader;
        code[RgbMaskFragmentShaderPass1] = qglslRgbMaskFragmentShaderPass1;
        code[RgbMaskFragmentShaderPass2] = qglslRgbMaskFragmentShaderPass2;
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
    simpleShaderProg = new QGLShaderProgram(context, this);
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
    blitShaderProg = new QGLShaderProgram(context, this);
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

void QGLEngineSharedShaders::shaderDestroyed(QObject *shader)
{
    // Remove any shader programs which has this as the srcPixel shader:
    for (int i = 0; i < cachedPrograms.size(); ++i) {
        if (cachedPrograms.at(i).srcPixelFragShader == shader) {
            delete cachedPrograms.at(i).program;
            cachedPrograms.removeAt(i--);
        }
    }

    emit shaderProgNeedsChanging();
}

QGLShader *QGLEngineSharedShaders::compileNamedShader(ShaderName name, QGLShader::ShaderType type)
{
    Q_ASSERT(name != CustomImageSrcFragmentShader);
    Q_ASSERT(name < InvalidShaderName);

    if (compiledShaders[name])
        return compiledShaders[name];

    QByteArray source = qglEngineShaderSourceCode[name];
    QGLShader *newShader = new QGLShader(type, ctxGuard.context(), this);
    newShader->compile(source);

#if defined(QT_DEBUG)
    // Name the shader for easier debugging
    QMetaEnum m = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("ShaderName"));
    newShader->setObjectName(QLatin1String(m.valueToKey(name)));
#endif

    compiledShaders[name] = newShader;
    return newShader;
}

QGLShader *QGLEngineSharedShaders::compileCustomShader(QGLCustomShaderStage *stage, QGLShader::ShaderType type)
{
    QByteArray source = stage->source();
    source += qglslCustomSrcFragmentShader;

    QGLShader *newShader = customShaderCache.object(source);
    if (newShader)
        return newShader;

    newShader = new QGLShader(type, ctxGuard.context(), this);
    newShader->compile(source);
    customShaderCache.insert(source, newShader);

    connect(newShader, SIGNAL(destroyed(QObject *)),
            this, SLOT(shaderDestroyed(QObject *)));

#if defined(QT_DEBUG)
    // Name the shader for easier debugging
    QMetaEnum m = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("ShaderName"));
    newShader->setObjectName(QLatin1String(m.valueToKey(CustomImageSrcFragmentShader)));
#endif

    return newShader;
}

// The address returned here will only be valid until next time this function is called.
QGLEngineShaderProg *QGLEngineSharedShaders::findProgramInCache(const QGLEngineShaderProg &prog)
{
    for (int i = 0; i < cachedPrograms.size(); ++i) {
        if (cachedPrograms[i] == prog)
            return &cachedPrograms[i];
    }

    cachedPrograms.append(prog);
    QGLEngineShaderProg &cached = cachedPrograms.last();

    // If the shader program's not found in the cache, create it now.
    cached.program = new QGLShaderProgram(ctxGuard.context(), this);
    cached.program->addShader(cached.mainVertexShader);
    cached.program->addShader(cached.positionVertexShader);
    cached.program->addShader(cached.mainFragShader);
    cached.program->addShader(cached.srcPixelFragShader);
    cached.program->addShader(cached.maskFragShader);
    cached.program->addShader(cached.compositionFragShader);

    // We have to bind the vertex attribute names before the program is linked:
    cached.program->bindAttributeLocation("vertexCoordsArray", QT_VERTEX_COORDS_ATTR);
    if (cached.useTextureCoords)
        cached.program->bindAttributeLocation("textureCoordArray", QT_TEXTURE_COORDS_ATTR);
    if (cached.useOpacityAttribute)
        cached.program->bindAttributeLocation("opacityArray", QT_OPACITY_ATTR);

    cached.program->link();
    if (!cached.program->isLinked()) {
        QLatin1String none("none");
        QLatin1String br("\n");
        QString error;
        error = QLatin1String("Shader program failed to link,")
#if defined(QT_DEBUG)
            + br
            + QLatin1String("  Shaders Used:\n")
            + QLatin1String("    mainVertexShader = ")
            + (cached.mainVertexShader ?
                cached.mainVertexShader->objectName() : none) + br
            + QLatin1String("    positionVertexShader = ")
            + (cached.positionVertexShader ?
                cached.positionVertexShader->objectName() : none) + br
            + QLatin1String("    mainFragShader = ")
            + (cached.mainFragShader ?
                cached.mainFragShader->objectName() : none) + br
            + QLatin1String("    srcPixelFragShader = ")
            + (cached.srcPixelFragShader ?
                cached.srcPixelFragShader->objectName() : none) + br
            + QLatin1String("    maskFragShader = ")
            + (cached.maskFragShader ?
                cached.maskFragShader->objectName() : none) + br
            + QLatin1String("    compositionFragShader = ")
            + (cached.compositionFragShader ?
                cached.compositionFragShader->objectName() : none) + br
#endif
            + QLatin1String("  Error Log:\n")
            + QLatin1String("    ") + cached.program->log();
        qWarning() << error;
        delete cached.program;
        cachedPrograms.removeLast();
        return 0;
    } else {
        // taking the address here is safe since
        // cachePrograms isn't resized anywhere else
        return &cached;
    }
}

QGLEngineShaderManager::QGLEngineShaderManager(QGLContext* context)
    : ctx(context),
      shaderProgNeedsChanging(true),
      srcPixelType(Qt::NoBrush),
      opacityMode(NoOpacity),
      maskType(NoMask),
      compositionMode(QPainter::CompositionMode_SourceOver),
      customSrcStage(0),
      currentShaderProg(0),
      customShader(0)
{
    sharedShaders = QGLEngineSharedShaders::shadersForContext(context);
    connect(sharedShaders, SIGNAL(shaderProgNeedsChanging()), this, SLOT(shaderProgNeedsChangingSlot()));
}

QGLEngineShaderManager::~QGLEngineShaderManager()
{
    //###
}

uint QGLEngineShaderManager::getUniformLocation(Uniform id)
{
    QVector<uint> &uniformLocations = currentShaderProg->uniformLocations;
    if (uniformLocations.isEmpty())
        uniformLocations.fill(GLuint(-1), NumUniforms);

    static const char *uniformNames[] = {
        "imageTexture",
        "patternColor",
        "globalOpacity",
        "depth",
        "pmvMatrix",
        "maskTexture",
        "fragmentColor",
        "linearData",
        "angle",
        "halfViewportSize",
        "fmp",
        "fmp2_m_radius2",
        "inverse_2_fmp2_m_radius2",
        "invertedTextureSize",
        "brushTransform",
        "brushTexture"
    };

    if (uniformLocations.at(id) == GLuint(-1))
        uniformLocations[id] = currentShaderProg->program->uniformLocation(uniformNames[id]);

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
    if (srcPixelType == PixelSrcType(style))
        return;

    srcPixelType = style;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setSrcPixelType(PixelSrcType type)
{
    if (srcPixelType == type)
        return;

    srcPixelType = type;
    shaderProgNeedsChanging = true; //###
}

void QGLEngineShaderManager::setOpacityMode(OpacityMode mode)
{
    if (opacityMode == mode)
        return;

    opacityMode = mode;
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

void QGLEngineShaderManager::setCustomStage(QGLCustomShaderStage* stage)
{
    customSrcStage = stage;
    customShader = 0; // Will be compiled from 'customSrcStage' later.
    shaderProgNeedsChanging = true;
}

void QGLEngineShaderManager::removeCustomStage(QGLCustomShaderStage* stage)
{
    Q_UNUSED(stage); // Currently we only support one at a time...

    customSrcStage = 0;
    customShader = 0;
    shaderProgNeedsChanging = true;
}


QGLShaderProgram* QGLEngineShaderManager::currentProgram()
{
    return currentShaderProg->program;
}

QGLShaderProgram* QGLEngineShaderManager::simpleProgram()
{
    return sharedShaders->simpleProgram();
}

QGLShaderProgram* QGLEngineShaderManager::blitProgram()
{
    return sharedShaders->blitProgram();
}



// Select & use the correct shader program using the current state.
// Returns true if program needed changing.
bool QGLEngineShaderManager::useCorrectShaderProg()
{
    if (!shaderProgNeedsChanging)
        return false;

    bool useCustomSrc = customSrcStage != 0;
    if (useCustomSrc && srcPixelType != QGLEngineShaderManager::ImageSrc) {
        useCustomSrc = false;
        qWarning("QGLEngineShaderManager - Ignoring custom shader stage for non image src");
    }

    QGLEngineShaderProg requiredProgram;
    requiredProgram.program = 0;

    bool texCoords = false;

    // Choose vertex shader shader position function (which typically also sets
    // varyings) and the source pixel (srcPixel) fragment shader function:
    QGLEngineSharedShaders::ShaderName positionVertexShaderName = QGLEngineSharedShaders::InvalidShaderName;
    QGLEngineSharedShaders::ShaderName srcPixelFragShaderName = QGLEngineSharedShaders::InvalidShaderName;
    bool isAffine = brushTransform.isAffine();
    if ( (srcPixelType >= Qt::Dense1Pattern) && (srcPixelType <= Qt::DiagCrossPattern) ) {
        if (isAffine)
            positionVertexShaderName = QGLEngineSharedShaders::AffinePositionWithPatternBrushVertexShader;
        else
            positionVertexShaderName = QGLEngineSharedShaders::PositionWithPatternBrushVertexShader;

        srcPixelFragShaderName = QGLEngineSharedShaders::PatternBrushSrcFragmentShader;
    }
    else switch (srcPixelType) {
        default:
        case Qt::NoBrush:
            qFatal("QGLEngineShaderManager::useCorrectShaderProg() - Qt::NoBrush style is set");
            break;
        case QGLEngineShaderManager::ImageSrc:
            srcPixelFragShaderName = QGLEngineSharedShaders::ImageSrcFragmentShader;
            positionVertexShaderName = QGLEngineSharedShaders::PositionOnlyVertexShader;
            texCoords = true;
            break;
        case QGLEngineShaderManager::NonPremultipliedImageSrc:
            srcPixelFragShaderName = QGLEngineSharedShaders::NonPremultipliedImageSrcFragmentShader;
            positionVertexShaderName = QGLEngineSharedShaders::PositionOnlyVertexShader;
            texCoords = true;
            break;
        case QGLEngineShaderManager::PatternSrc:
            srcPixelFragShaderName = QGLEngineSharedShaders::ImageSrcWithPatternFragmentShader;
            positionVertexShaderName = QGLEngineSharedShaders::PositionOnlyVertexShader;
            texCoords = true;
            break;
        case QGLEngineShaderManager::TextureSrcWithPattern:
            srcPixelFragShaderName = QGLEngineSharedShaders::TextureBrushSrcWithPatternFragmentShader;
            positionVertexShaderName = isAffine ? QGLEngineSharedShaders::AffinePositionWithTextureBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithTextureBrushVertexShader;
            break;
        case Qt::SolidPattern:
            srcPixelFragShaderName = QGLEngineSharedShaders::SolidBrushSrcFragmentShader;
            positionVertexShaderName = QGLEngineSharedShaders::PositionOnlyVertexShader;
            break;
        case Qt::LinearGradientPattern:
            srcPixelFragShaderName = QGLEngineSharedShaders::LinearGradientBrushSrcFragmentShader;
            positionVertexShaderName = isAffine ? QGLEngineSharedShaders::AffinePositionWithLinearGradientBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithLinearGradientBrushVertexShader;
            break;
        case Qt::ConicalGradientPattern:
            srcPixelFragShaderName = QGLEngineSharedShaders::ConicalGradientBrushSrcFragmentShader;
            positionVertexShaderName = isAffine ? QGLEngineSharedShaders::AffinePositionWithConicalGradientBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithConicalGradientBrushVertexShader;
            break;
        case Qt::RadialGradientPattern:
            srcPixelFragShaderName = QGLEngineSharedShaders::RadialGradientBrushSrcFragmentShader;
            positionVertexShaderName = isAffine ? QGLEngineSharedShaders::AffinePositionWithRadialGradientBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithRadialGradientBrushVertexShader;
            break;
        case Qt::TexturePattern:
            srcPixelFragShaderName = QGLEngineSharedShaders::TextureBrushSrcFragmentShader;
            positionVertexShaderName = isAffine ? QGLEngineSharedShaders::AffinePositionWithTextureBrushVertexShader
                                                : QGLEngineSharedShaders::PositionWithTextureBrushVertexShader;
            break;
    };
    requiredProgram.positionVertexShader = sharedShaders->compileNamedShader(positionVertexShaderName, QGLShader::PartialVertexShader);
    if (useCustomSrc) {
        if (!customShader)
            customShader = sharedShaders->compileCustomShader(customSrcStage, QGLShader::PartialFragmentShader);
        requiredProgram.srcPixelFragShader = customShader;
    } else {
        requiredProgram.srcPixelFragShader = sharedShaders->compileNamedShader(srcPixelFragShaderName, QGLShader::PartialFragmentShader);
    }

    const bool hasCompose = compositionMode > QPainter::CompositionMode_Plus;
    const bool hasMask = maskType != QGLEngineShaderManager::NoMask;

    // Choose fragment shader main function:
    QGLEngineSharedShaders::ShaderName mainFragShaderName;

    if (opacityMode == AttributeOpacity) {
        Q_ASSERT(!hasCompose && !hasMask);
        mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader_ImageArrays;
    } else {
        bool useGlobalOpacity = (opacityMode == UniformOpacity);
        if (hasCompose && hasMask && useGlobalOpacity)
            mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader_CMO;
        if (hasCompose && hasMask && !useGlobalOpacity)
            mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader_CM;
        if (!hasCompose && hasMask && useGlobalOpacity)
            mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader_MO;
        if (!hasCompose && hasMask && !useGlobalOpacity)
            mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader_M;
        if (hasCompose && !hasMask && useGlobalOpacity)
            mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader_CO;
        if (hasCompose && !hasMask && !useGlobalOpacity)
            mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader_C;
        if (!hasCompose && !hasMask && useGlobalOpacity)
            mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader_O;
        if (!hasCompose && !hasMask && !useGlobalOpacity)
            mainFragShaderName = QGLEngineSharedShaders::MainFragmentShader;
    }

    requiredProgram.mainFragShader = sharedShaders->compileNamedShader(mainFragShaderName, QGLShader::PartialFragmentShader);

    if (hasMask) {
        QGLEngineSharedShaders::ShaderName maskShaderName = QGLEngineSharedShaders::InvalidShaderName;
        if (maskType == PixelMask) {
            maskShaderName = QGLEngineSharedShaders::MaskFragmentShader;
            texCoords = true;
        } else if (maskType == SubPixelMaskPass1) {
            maskShaderName = QGLEngineSharedShaders::RgbMaskFragmentShaderPass1;
            texCoords = true;
        } else if (maskType == SubPixelMaskPass2) {
            maskShaderName = QGLEngineSharedShaders::RgbMaskFragmentShaderPass2;
            texCoords = true;
        } else if (maskType == SubPixelWithGammaMask) {
            maskShaderName = QGLEngineSharedShaders::RgbMaskWithGammaFragmentShader;
            texCoords = true;
        } else {
            qCritical("QGLEngineShaderManager::useCorrectShaderProg() - Unknown mask type");
        }

        requiredProgram.maskFragShader = sharedShaders->compileNamedShader(maskShaderName, QGLShader::PartialFragmentShader);
    } else {
        requiredProgram.maskFragShader = 0;
    }

    if (hasCompose) {
        QGLEngineSharedShaders::ShaderName compositionShaderName = QGLEngineSharedShaders::InvalidShaderName;
        switch (compositionMode) {
            case QPainter::CompositionMode_Multiply:
                compositionShaderName = QGLEngineSharedShaders::MultiplyCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Screen:
                compositionShaderName = QGLEngineSharedShaders::ScreenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Overlay:
                compositionShaderName = QGLEngineSharedShaders::OverlayCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Darken:
                compositionShaderName = QGLEngineSharedShaders::DarkenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Lighten:
                compositionShaderName = QGLEngineSharedShaders::LightenCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_ColorDodge:
                compositionShaderName = QGLEngineSharedShaders::ColorDodgeCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_ColorBurn:
                compositionShaderName = QGLEngineSharedShaders::ColorBurnCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_HardLight:
                compositionShaderName = QGLEngineSharedShaders::HardLightCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_SoftLight:
                compositionShaderName = QGLEngineSharedShaders::SoftLightCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Difference:
                compositionShaderName = QGLEngineSharedShaders::DifferenceCompositionModeFragmentShader;
                break;
            case QPainter::CompositionMode_Exclusion:
                compositionShaderName = QGLEngineSharedShaders::ExclusionCompositionModeFragmentShader;
                break;
            default:
                qWarning("QGLEngineShaderManager::useCorrectShaderProg() - Unsupported composition mode");
        }
        requiredProgram.compositionFragShader = sharedShaders->compileNamedShader(compositionShaderName, QGLShader::PartialFragmentShader);
    } else {
        requiredProgram.compositionFragShader = 0;
    }

        // Choose vertex shader main function
    QGLEngineSharedShaders::ShaderName mainVertexShaderName = QGLEngineSharedShaders::InvalidShaderName;
    if (opacityMode == AttributeOpacity) {
        Q_ASSERT(texCoords);
        mainVertexShaderName = QGLEngineSharedShaders::MainWithTexCoordsAndOpacityVertexShader;
    } else if (texCoords) {
        mainVertexShaderName = QGLEngineSharedShaders::MainWithTexCoordsVertexShader;
    } else {
        mainVertexShaderName = QGLEngineSharedShaders::MainVertexShader;
    }
    requiredProgram.mainVertexShader = sharedShaders->compileNamedShader(mainVertexShaderName, QGLShader::PartialVertexShader);
    requiredProgram.useTextureCoords = texCoords;
    requiredProgram.useOpacityAttribute = (opacityMode == AttributeOpacity);


    // At this point, requiredProgram is fully populated so try to find the program in the cache
    currentShaderProg = sharedShaders->findProgramInCache(requiredProgram);

    if (currentShaderProg) {
        currentShaderProg->program->enable();
        if (useCustomSrc)
            customSrcStage->setUniforms(currentShaderProg->program);
    }

    shaderProgNeedsChanging = false;
    return true;
}

QT_END_NAMESPACE
