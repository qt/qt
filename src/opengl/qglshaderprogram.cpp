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

#include "qglshaderprogram.h"
#include "qglextensions_p.h"
#include "qgl_p.h"
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)

/*!
    \class QGLShaderProgram
    \brief The QGLShaderProgram class allows OpenGL shader programs to be linked and used.
    \since 4.6
    \ingroup painting-3D

    \section1 Introduction

    This class supports shader programs written in the OpenGL Shading
    Language (GLSL) and in the OpenGL/ES Shading Language (GLSL/ES).

    QGLShader and QGLShaderProgram shelter the programmer from the details of
    compiling and linking vertex and fragment shaders.

    The following example creates a vertex shader program using the
    supplied source \c{code}.  Once compiled and linked, the shader
    program is activated in the current QGLContext by calling
    QGLShaderProgram::enable():

    \snippet doc/src/snippets/code/src_opengl_qglshaderprogram.cpp 0

    \section1 Writing portable shaders

    Shader programs can be difficult to reuse across OpenGL implementations
    because of varying levels of support for standard vertex attributes and
    uniform variables.  In particular, GLSL/ES lacks all of the
    standard variables that are present on desktop OpenGL systems:
    \c{gl_Vertex}, \c{gl_Normal}, \c{gl_Color}, and so on.  Desktop OpenGL
    lacks the variable qualifiers \c{highp}, \c{mediump}, and \c{lowp}.

    The QGLShaderProgram class makes the process of writing portable shaders
    easier by prefixing all shader programs with the following lines on
    desktop OpenGL:

    \code
    #define highp
    #define mediump
    #define lowp
    \endcode

    This makes it possible to run most GLSL/ES shader programs
    on desktop systems.  The programmer should restrict themselves
    to just features that are present in GLSL/ES, and avoid
    standard variable names that only work on the desktop.

    \section1 Simple shader example

    \snippet doc/src/snippets/code/src_opengl_qglshaderprogram.cpp 1

    With the above shader program active, we can draw a green triangle
    as follows:

    \snippet doc/src/snippets/code/src_opengl_qglshaderprogram.cpp 2

    \section1 Partial shaders

    Desktop GLSL can attach an arbitrary number of vertex and fragment
    shaders to a shader program.  Embedded GLSL/ES on the other hand
    supports only a single shader of each type on a shader program.

    Multiple shaders of the same type can be useful when large libraries
    of shaders are needed.  Common functions can be factored out into
    library shaders that can be reused in multiple shader programs.

    To support this use of shaders, the application programmer can
    create shaders with the QGLShader::PartialVertexShader and
    QGLShader::PartialFragmentShader types.  These types direct
    QGLShader and QGLShaderProgram to delay shader compilation until
    link time.

    When link() is called, the sources for the partial shaders are
    concatenated, and a single vertex or fragment shader is compiled
    and linked into the shader program.

    It is more efficient to use the QGLShader::VertexShader and
    QGLShader::FragmentShader when there is only one shader of that
    type in the program.

    \sa QGLShader
*/

/*!
    \class QGLShader
    \brief The QGLShader class allows OpenGL shaders to be compiled.
    \since 4.6
    \ingroup painting-3D

    This class supports shaders written in the OpenGL Shading Language (GLSL)
    and in the OpenGL/ES Shading Language (GLSL/ES).

    QGLShader and QGLShaderProgram shelter the programmer from the details of
    compiling and linking vertex and fragment shaders.

    \sa QGLShaderProgram
*/

/*!
    \enum QGLShader::ShaderTypeBits
    This enum specifies the type of QGLShader that is being created.

    \value VertexShader Vertex shader written in the OpenGL Shading Language (GLSL).
    \value FragmentShader Fragment shader written in the OpenGL Shading Language (GLSL).

    \value PartialVertexShader Partial vertex shader that will be concatenated with all other partial vertex shaders at link time.
    \value PartialFragmentShader Partial fragment shader that will be concatenated with all other partial fragment shaders at link time.

    \omitvalue PartialShader
*/

#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif
#ifndef GL_ACTIVE_UNIFORMS
#define GL_ACTIVE_UNIFORMS 0x8B86
#endif
#ifndef GL_ACTIVE_UNIFORM_MAX_LENGTH
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#endif
#ifndef GL_ACTIVE_ATTRIBUTES
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#endif
#ifndef GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#endif
#ifndef GL_CURRENT_VERTEX_ATTRIB
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#endif
#ifndef GL_SHADER_SOURCE_LENGTH
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#endif
#ifndef GL_SHADER_BINARY_FORMATS
#define GL_SHADER_BINARY_FORMATS          0x8DF8
#endif
#ifndef GL_NUM_SHADER_BINARY_FORMATS
#define GL_NUM_SHADER_BINARY_FORMATS      0x8DF9
#endif

class QGLShaderPrivate
{
public:
    QGLShaderPrivate(const QGLContext *context, QGLShader::ShaderType type)
        : shaderGuard(context)
        , shaderType(type)
        , compiled(false)
        , isPartial((type & QGLShader::PartialShader) != 0)
        , hasPartialSource(false)
    {
    }

    QGLSharedResourceGuard shaderGuard;
    QGLShader::ShaderType shaderType;
    bool compiled;
    bool isPartial;
    bool hasPartialSource;
    QString log;
    QByteArray partialSource;

    bool create();
    bool compile(QGLShader *q);
    void deleteShader();
};

#define ctx shaderGuard.context()

bool QGLShaderPrivate::create()
{
    const QGLContext *context = shaderGuard.context();
    if (!context)
        return false;
    if (isPartial)
        return true;
    if (qt_resolve_glsl_extensions(const_cast<QGLContext *>(context))) {
        GLuint shader;
        if (shaderType == QGLShader::VertexShader)
            shader = glCreateShader(GL_VERTEX_SHADER);
        else
            shader = glCreateShader(GL_FRAGMENT_SHADER);
        if (!shader) {
            qWarning() << "QGLShader: could not create shader";
            return false;
        }
        shaderGuard.setId(shader);
        return true;
    } else {
        return false;
    }
}

bool QGLShaderPrivate::compile(QGLShader *q)
{
    // Partial shaders are compiled during QGLShaderProgram::link().
    if (isPartial && hasPartialSource) {
        compiled = true;
        return true;
    }
    GLuint shader = shaderGuard.id();
    if (!shader)
        return false;
    glCompileShader(shader);
    GLint value = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
    compiled = (value != 0);
    value = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &value);
    if (!compiled && value > 1) {
        char *logbuf = new char [value];
        GLint len;
        glGetShaderInfoLog(shader, value, &len, logbuf);
        log = QString::fromLatin1(logbuf);
        QString name = q->objectName();
        if (name.isEmpty())
            qWarning() << "QGLShader::compile:" << log;
        else
            qWarning() << "QGLShader::compile[" << name << "]:" << log;
        delete [] logbuf;
    }
    return compiled;
}

void QGLShaderPrivate::deleteShader()
{
    if (shaderGuard.id()) {
        glDeleteShader(shaderGuard.id());
        shaderGuard.setId(0);
    }
}

#undef ctx
#define ctx d->shaderGuard.context()

/*!
    Constructs a new QGLShader object of the specified \a type
    and attaches it to \a parent.  If shader programs are not supported,
    QGLShaderProgram::hasShaderPrograms() will return false.

    This constructor is normally followed by a call to compile()
    or compileFile().

    The shader will be associated with the current QGLContext.

    \sa compile(), compileFile()
*/
QGLShader::QGLShader(QGLShader::ShaderType type, QObject *parent)
    : QObject(parent)
{
    d = new QGLShaderPrivate(QGLContext::currentContext(), type);
    d->create();
}

/*!
    Constructs a new QGLShader object of the specified \a type from the
    source code in \a fileName and attaches it to \a parent.
    If the shader could not be loaded, then isCompiled() will return false.

    The shader will be associated with the current QGLContext.

    \sa isCompiled()
*/
QGLShader::QGLShader
        (const QString& fileName, QGLShader::ShaderType type, QObject *parent)
    : QObject(parent)
{
    d = new QGLShaderPrivate(QGLContext::currentContext(), type);
    if (d->create() && !compileFile(fileName))
        d->deleteShader();
}

/*!
    Constructs a new QGLShader object of the specified \a type
    and attaches it to \a parent.  If shader programs are not supported,
    then QGLShaderProgram::hasShaderPrograms() will return false.

    This constructor is normally followed by a call to compile()
    or compileFile().

    The shader will be associated with \a context.

    \sa compile(), compileFile()
*/
QGLShader::QGLShader(QGLShader::ShaderType type, const QGLContext *context, QObject *parent)
    : QObject(parent)
{
    if (!context)
        context = QGLContext::currentContext();
    d = new QGLShaderPrivate(context, type);
#ifndef QT_NO_DEBUG
    if (context && !QGLContext::areSharing(context, QGLContext::currentContext())) {
        qWarning("QGLShader::QGLShader: \'context\' must be the currect context or sharing with it.");
        return;
    }
#endif
    d->create();
}

/*!
    Constructs a new QGLShader object of the specified \a type from the
    source code in \a fileName and attaches it to \a parent.
    If the shader could not be loaded, then isCompiled() will return false.

    The shader will be associated with \a context.

    \sa isCompiled()
*/
QGLShader::QGLShader
        (const QString& fileName, QGLShader::ShaderType type, const QGLContext *context, QObject *parent)
    : QObject(parent)
{
    if (!context)
        context = QGLContext::currentContext();
    d = new QGLShaderPrivate(context, type);
#ifndef QT_NO_DEBUG
    if (context && !QGLContext::areSharing(context, QGLContext::currentContext())) {
        qWarning("QGLShader::QGLShader: \'context\' must be currect context or sharing with it.");
        return;
    }
#endif
    if (d->create() && !compileFile(fileName))
        d->deleteShader();
}

/*!
    Deletes this shader.  If the shader has been attached to a
    QGLShaderProgram object, then the actual shader will stay around
    until the QGLShaderProgram is destroyed.
*/
QGLShader::~QGLShader()
{
    if (d->shaderGuard.id()) {
        QGLShareContextScope scope(d->shaderGuard.context());
        glDeleteShader(d->shaderGuard.id());
    }
    delete d;
}

/*!
    Returns the type of this shader.
*/
QGLShader::ShaderType QGLShader::shaderType() const
{
    return d->shaderType;
}

// The precision qualifiers are useful on OpenGL/ES systems,
// but usually not present on desktop systems.  Define the
// keywords to empty strings on desktop systems.
#ifndef QT_OPENGL_ES
#define QGL_DEFINE_QUALIFIERS 1
static const char qualifierDefines[] =
    "#define lowp\n"
    "#define mediump\n"
    "#define highp\n";
#endif

// The "highp" qualifier doesn't exist in fragment shaders
// on all ES platforms.  When it doesn't exist, use "mediump".
#ifdef QT_OPENGL_ES
#define QGL_REDEFINE_HIGHP 1
static const char redefineHighp[] =
    "#ifndef GL_FRAGMENT_PRECISION_HIGH\n"
    "#define highp mediump\n"
    "#endif\n";
#endif

/*!
    Sets the \a source code for this shader and compiles it.
    Returns true if the source was successfully compiled, false otherwise.

    If shaderType() is PartialVertexShader or PartialFragmentShader,
    then this function will always return true, even if the source code
    is invalid.  Partial shaders are compiled when QGLShaderProgram::link()
    is called.

    \sa compileFile()
*/
bool QGLShader::compile(const char *source)
{
    if (d->isPartial) {
        d->partialSource = QByteArray(source);
        d->hasPartialSource = true;
        return d->compile(this);
    } else if (d->shaderGuard.id()) {
        QVarLengthArray<const char *> src;
        int headerLen = 0;
        while (source && source[headerLen] == '#') {
            // Skip #version and #extension directives at the start of
            // the shader code.  We need to insert the qualifierDefines
            // and redefineHighp just after them.
            if (qstrncmp(source + headerLen, "#version", 8) != 0 &&
                    qstrncmp(source + headerLen, "#extension", 10) != 0) {
                break;
            }
            while (source[headerLen] != '\0' && source[headerLen] != '\n')
                ++headerLen;
            if (source[headerLen] == '\n')
                ++headerLen;
        }
        QByteArray header;
        if (headerLen > 0) {
            header = QByteArray(source, headerLen);
            src.append(header.constData());
        }
#ifdef QGL_DEFINE_QUALIFIERS
        src.append(qualifierDefines);
#endif
#ifdef QGL_REDEFINE_HIGHP
        if (d->shaderType == FragmentShader ||
                d->shaderType == PartialFragmentShader)
            src.append(redefineHighp);
#endif
        src.append(source + headerLen);
        glShaderSource(d->shaderGuard.id(), src.size(), src.data(), 0);
        return d->compile(this);
    } else {
        return false;
    }
}

/*!
    \overload

    Sets the \a source code for this shader and compiles it.
    Returns true if the source was successfully compiled, false otherwise.

    If shaderType() is PartialVertexShader or PartialFragmentShader,
    then this function will always return true, even if the source code
    is invalid.  Partial shaders are compiled when QGLShaderProgram::link()
    is called.

    \sa compileFile()
*/
bool QGLShader::compile(const QByteArray& source)
{
    return compile(source.constData());
}

/*!
    \overload

    Sets the \a source code for this shader and compiles it.
    Returns true if the source was successfully compiled, false otherwise.

    If shaderType() is PartialVertexShader or PartialFragmentShader,
    then this function will always return true, even if the source code
    is invalid.  Partial shaders are compiled when QGLShaderProgram::link()
    is called.

    \sa compileFile()
*/
bool QGLShader::compile(const QString& source)
{
    return compile(source.toLatin1().constData());
}

/*!
    Sets the source code for this shader to the contents of \a fileName
    and compiles it.  Returns true if the file could be opened and the
    source compiled, false otherwise.

    If shaderType() is PartialVertexShader or PartialFragmentShader,
    then this function will always return true, even if the source code
    is invalid.  Partial shaders are compiled when QGLShaderProgram::link()
    is called.

    \sa compile()
*/
bool QGLShader::compileFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "QGLShader: Unable to open file" << fileName;
        return false;
    }

    QByteArray contents = file.readAll();
    return compile(contents.constData());
}

/*!
    Sets the binary code for this shader to the \a length bytes from
    the array \a binary.  The \a format specifies how the binary data
    should be interpreted by the OpenGL engine.  Returns true if the
    binary was set on the shader; false otherwise.

    This function cannot be used with PartialVertexShader or
    PartialFragmentShader.

    If this function succeeds, then the shader will be considered compiled.

    \sa shaderBinaryFormats()
*/
bool QGLShader::setShaderBinary(GLenum format, const void *binary, int length)
{
#if !defined(QT_OPENGL_ES_2)
    if (!glShaderBinary)
        return false;
#endif
    GLuint shader = d->shaderGuard.id();
    if (d->isPartial || !shader)
        return false;
    glGetError();   // Clear error state.
    glShaderBinary(1, &shader, format, binary, length);
    d->compiled = (glGetError() == GL_NO_ERROR);
    return d->compiled;
}

/*!
    Sets the binary code for this shader to the \a length bytes from
    the array \a binary.  The \a format specifies how the binary data
    should be interpreted by the OpenGL engine.  Returns true if the
    binary was set on the shader; false otherwise.

    The \a otherShader will also have binary code set on it.  This is
    for the case where \a binary contains both vertex and fragment
    shader code.

    This function cannot be used with PartialVertexShader or
    PartialFragmentShader.

    If this function succeeds, then the shader will be considered compiled.

    \sa shaderBinaryFormats()
*/
bool QGLShader::setShaderBinary
    (QGLShader& otherShader, GLenum format, const void *binary, int length)
{
#if !defined(QT_OPENGL_ES_2)
    if (!glShaderBinary)
        return false;
#endif
    if (d->isPartial || !d->shaderGuard.id())
        return false;
    if (otherShader.d->isPartial || !otherShader.d->shaderGuard.id())
        return false;
    glGetError();   // Clear error state.
    GLuint shaders[2];
    shaders[0] = d->shaderGuard.id();
    shaders[1] = otherShader.d->shaderGuard.id();
    glShaderBinary(2, shaders, format, binary, length);
    d->compiled = (glGetError() == GL_NO_ERROR);
    otherShader.d->compiled = d->compiled;
    return d->compiled;
}

/*!
    Returns a list of all binary formats that are supported by
    setShaderBinary() on this system.

    \sa setShaderBinary()
*/
QList<GLenum> QGLShader::shaderBinaryFormats()
{
    GLint num;
    QList<GLenum> list;
    glGetError();   // Clear error state.
    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &num);
    if (glGetError() != GL_NO_ERROR || num <= 0)
        return list;
    QVarLengthArray<GLint> formats(num);
    glGetIntegerv(GL_SHADER_BINARY_FORMATS, formats.data());
    for (GLint i = 0; i < num; ++i)
        list += (GLenum)(formats[i]);
    return list;
}

/*!
    Returns the source code for this shader.

    \sa compile()
*/
QByteArray QGLShader::sourceCode() const
{
    if (d->isPartial)
        return d->partialSource;
    GLuint shader = d->shaderGuard.id();
    if (!shader)
        return QByteArray();
    GLint size = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &size);
    if (size <= 0)
        return QByteArray();
    GLint len = 0;
    char *source = new char [size];
    glGetShaderSource(shader, size, &len, source);
    QByteArray src(source);
    delete [] source;
    return src;
}

/*!
    Returns true if this shader has been compiled; false otherwise.

    \sa compile()
*/
bool QGLShader::isCompiled() const
{
    return d->compiled;
}

/*!
    Returns the errors and warnings that occurred during the last compile.

    \sa compile()
*/
QString QGLShader::log() const
{
    return d->log;
}

/*!
    Returns the OpenGL identifier associated with this shader.

    If shaderType() is PartialVertexShader or PartialFragmentShader,
    this function will always return zero.  Partial shaders are
    created and compiled when QGLShaderProgram::link() is called.

    \sa QGLShaderProgram::programId()
*/
GLuint QGLShader::shaderId() const
{
    return d->shaderGuard.id();
}

#undef ctx
#define ctx programGuard.context()

class QGLShaderProgramPrivate
{
public:
    QGLShaderProgramPrivate(const QGLContext *context)
        : programGuard(context)
        , linked(false)
        , inited(false)
        , hasPartialShaders(false)
        , removingShaders(false)
        , vertexShader(0)
        , fragmentShader(0)
    {
    }
    ~QGLShaderProgramPrivate();

    QGLSharedResourceGuard programGuard;
    bool linked;
    bool inited;
    bool hasPartialShaders;
    bool removingShaders;
    QString log;
    QList<QGLShader *> shaders;
    QList<QGLShader *> anonShaders;
    QGLShader *vertexShader;
    QGLShader *fragmentShader;
};

QGLShaderProgramPrivate::~QGLShaderProgramPrivate()
{
    if (programGuard.id()) {
        QGLShareContextScope scope(programGuard.context());
        glDeleteProgram(programGuard.id());
    }
}

#undef ctx
#define ctx d->programGuard.context()

/*!
    Constructs a new shader program and attaches it to \a parent.
    The program will be invalid until addShader() is called.

    The shader program will be associated with the current QGLContext.

    \sa addShader()
*/
QGLShaderProgram::QGLShaderProgram(QObject *parent)
    : QObject(parent)
{
    d = new QGLShaderProgramPrivate(QGLContext::currentContext());
}

/*!
    Constructs a new shader program and attaches it to \a parent.
    The program will be invalid until addShader() is called.

    The shader program will be associated with \a context.

    \sa addShader()
*/
QGLShaderProgram::QGLShaderProgram(const QGLContext *context, QObject *parent)
    : QObject(parent)
{
    d = new QGLShaderProgramPrivate(context);
}

/*!
    Deletes this shader program.
*/
QGLShaderProgram::~QGLShaderProgram()
{
    delete d;
}

bool QGLShaderProgram::init()
{
    if (d->programGuard.id() || d->inited)
        return true;
    d->inited = true;
    const QGLContext *context = d->programGuard.context();
    if (!context) {
        context = QGLContext::currentContext();
        d->programGuard.setContext(context);
    }
    if (!context)
        return false;
    if (qt_resolve_glsl_extensions(const_cast<QGLContext *>(context))) {
        GLuint program = glCreateProgram();
        if (!program) {
            qWarning() << "QGLShaderProgram: could not create shader program";
            return false;
        }
        d->programGuard.setId(program);
        return true;
    } else {
        qWarning() << "QGLShaderProgram: shader programs are not supported";
        return false;
    }
}

/*!
    Adds a compiled \a shader to this shader program.  Returns true
    if the shader could be added, or false otherwise.

    Ownership of the \a shader object remains with the caller.
    It will not be deleted when this QGLShaderProgram instance
    is deleted.  This allows the caller to add the same shader
    to multiple shader programs.

    \sa removeShader(), link(), removeAllShaders()
*/
bool QGLShaderProgram::addShader(QGLShader *shader)
{
    if (!init())
        return false;
    if (d->shaders.contains(shader))
        return true;    // Already added to this shader program.
    if (d->programGuard.id() && shader) {
        if (!QGLContext::areSharing(shader->d->shaderGuard.context(),
                                    d->programGuard.context())) {
            qWarning("QGLShaderProgram::addShader: Program and shader are not associated with same context.");
            return false;
        }
        if (!shader->d->compiled)
            return false;
        if (!shader->d->isPartial) {
            if (!shader->d->shaderGuard.id())
                return false;
            glAttachShader(d->programGuard.id(), shader->d->shaderGuard.id());
        } else {
            d->hasPartialShaders = true;
        }
        d->linked = false;  // Program needs to be relinked.
        d->shaders.append(shader);
        connect(shader, SIGNAL(destroyed()), this, SLOT(shaderDestroyed()));
        return true;
    } else {
        return false;
    }
}

/*!
    Compiles \a source as a shader of the specified \a type and
    adds it to this shader program.  Returns true if compilation
    was successful, false otherwise.  The compilation errors
    and warnings will be made available via log().

    This function is intended to be a short-cut for quickly
    adding vertex and fragment shaders to a shader program without
    creating an instance of QGLShader first.

    \sa removeShader(), link(), log(), removeAllShaders()
*/
bool QGLShaderProgram::addShader(QGLShader::ShaderType type, const char *source)
{
    if (!init())
        return false;
    QGLShader *shader = new QGLShader(type, this);
    if (!shader->compile(source)) {
        d->log = shader->log();
        delete shader;
        return false;
    }
    d->anonShaders.append(shader);
    return addShader(shader);
}

/*!
    \overload

    Compiles \a source as a shader of the specified \a type and
    adds it to this shader program.  Returns true if compilation
    was successful, false otherwise.  The compilation errors
    and warnings will be made available via log().

    This function is intended to be a short-cut for quickly
    adding vertex and fragment shaders to a shader program without
    creating an instance of QGLShader first.

    \sa removeShader(), link(), log(), removeAllShaders()
*/
bool QGLShaderProgram::addShader(QGLShader::ShaderType type, const QByteArray& source)
{
    return addShader(type, source.constData());
}

/*!
    \overload

    Compiles \a source as a shader of the specified \a type and
    adds it to this shader program.  Returns true if compilation
    was successful, false otherwise.  The compilation errors
    and warnings will be made available via log().

    This function is intended to be a short-cut for quickly
    adding vertex and fragment shaders to a shader program without
    creating an instance of QGLShader first.

    \sa removeShader(), link(), log(), removeAllShaders()
*/
bool QGLShaderProgram::addShader(QGLShader::ShaderType type, const QString& source)
{
    return addShader(type, source.toLatin1().constData());
}

/*!
    Compiles the contents of \a fileName as a shader of the specified
    \a type and adds it to this shader program.  Returns true if
    compilation was successful, false otherwise.  The compilation errors
    and warnings will be made available via log().

    This function is intended to be a short-cut for quickly
    adding vertex and fragment shaders to a shader program without
    creating an instance of QGLShader first.

    \sa addShader()
*/
bool QGLShaderProgram::addShaderFromFile
    (QGLShader::ShaderType type, const QString& fileName)
{
    if (!init())
        return false;
    QGLShader *shader = new QGLShader(type, this);
    if (!shader->compileFile(fileName)) {
        d->log = shader->log();
        delete shader;
        return false;
    }
    d->anonShaders.append(shader);
    return addShader(shader);
}

/*!
    Removes \a shader from this shader program.  The object is not deleted.

    \sa addShader(), link(), removeAllShaders()
*/
void QGLShaderProgram::removeShader(QGLShader *shader)
{
    if (d->programGuard.id() && shader && shader->d->shaderGuard.id()) {
        QGLShareContextScope scope(d->programGuard.context());
        glDetachShader(d->programGuard.id(), shader->d->shaderGuard.id());
    }
    d->linked = false;  // Program needs to be relinked.
    if (shader) {
        d->shaders.removeAll(shader);
        d->anonShaders.removeAll(shader);
        disconnect(shader, SIGNAL(destroyed()), this, SLOT(shaderDestroyed()));
    }
}

/*!
    Returns a list of all shaders that have been added to this shader
    program using addShader().

    \sa addShader(), removeShader()
*/
QList<QGLShader *> QGLShaderProgram::shaders() const
{
    return d->shaders;
}

/*!
    Removes all of the shaders that were added to this program previously.
    The QGLShader objects for the shaders will not be deleted if they
    were constructed externally.  QGLShader objects that are constructed
    internally by QGLShaderProgram will be deleted.

    \sa addShader(), removeShader()
*/
void QGLShaderProgram::removeAllShaders()
{
    d->removingShaders = true;
    foreach (QGLShader *shader, d->shaders) {
        if (d->programGuard.id() && shader && shader->d->shaderGuard.id())
            glDetachShader(d->programGuard.id(), shader->d->shaderGuard.id());
    }
    foreach (QGLShader *shader, d->anonShaders) {
        // Delete shader objects that were created anonymously.
        delete shader;
    }
    d->shaders.clear();
    d->anonShaders.clear();
    d->linked = false;  // Program needs to be relinked.
    d->removingShaders = false;
}

#if defined(QT_OPENGL_ES_2)

#ifndef GL_PROGRAM_BINARY_LENGTH_OES
#define GL_PROGRAM_BINARY_LENGTH_OES        0x8741
#endif
#ifndef GL_NUM_PROGRAM_BINARY_FORMATS_OES
#define GL_NUM_PROGRAM_BINARY_FORMATS_OES   0x87FE
#endif
#ifndef GL_PROGRAM_BINARY_FORMATS_OES
#define GL_PROGRAM_BINARY_FORMATS_OES       0x87FF
#endif

#endif

/*!
    Returns the program binary associated with this shader program.
    The numeric identifier of the program binary format is returned
    in \a format.  The \c OES_get_program_binary extension will need
    to be supported by the system for binary retrieval to succeed.

    Returns an empty QByteArray if the program binary cannot be
    retrieved on this system, or the shader program has not yet
    been linked.

    The returned binary can be supplied to setProgramBinary() on the
    same machine at some future point to reload the program.  It contains
    the compiled code of all of the shaders that were attached to the
    program at the time programBinary() is called.

    \sa setProgramBinary(), programBinaryFormats()
*/
QByteArray QGLShaderProgram::programBinary(int *format) const
{
#if defined(QT_OPENGL_ES_2)
    if (!isLinked())
        return QByteArray();

    // Get the length of the binary data, bailing out if there is none.
    GLint length = 0;
    GLuint program = d->programGuard.id();
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH_OES, &length);
    if (length <= 0)
        return QByteArray();

    // Retrieve the binary data.
    QByteArray binary(length, 0);
    GLenum binaryFormat;
    glGetProgramBinaryOES(program, length, 0, &binaryFormat, binary.data());
    if (format)
        *format = (int)binaryFormat;
    return binary;
#else
    Q_UNUSED(format);
    return QByteArray();
#endif
}

/*!
    Sets the \a binary for this shader program according to \a format.
    Returns true if the binary was set, or false if the binary format
    is not supported or this system does not support program binaries.
    The program will be linked if the load succeeds.

    \sa programBinary(), programBinaryFormats(), isLinked()
*/
bool QGLShaderProgram::setProgramBinary(int format, const QByteArray& binary)
{
#if defined(QT_OPENGL_ES_2)
    // Load the binary and check that it was linked correctly.
    GLuint program = d->programGuard.id();
    if (!program)
        return false;
    glProgramBinaryOES(program, (GLenum)format,
                       binary.constData(), binary.size());
    GLint value = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &value);
    d->linked = (value != 0);
    value = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &value);
    d->log = QString();
    if (value > 1) {
        char *logbuf = new char [value];
        GLint len;
        glGetProgramInfoLog(program, value, &len, logbuf);
        d->log = QString::fromLatin1(logbuf);
        QString name = objectName();
        if (name.isEmpty())
            qWarning() << "QGLShader::setProgramBinary:" << d->log;
        else
            qWarning() << "QGLShader::setProgramBinary[" << name << "]:" << d->log;
        delete [] logbuf;
    }
    return d->linked;
#else
    Q_UNUSED(format);
    Q_UNUSED(binary);
    return false;
#endif
}

/*!
    Returns the list of program binary formats that are accepted by
    this system for use with setProgramBinary().

    \sa programBinary(), setProgramBinary()
*/
QList<int> QGLShaderProgram::programBinaryFormats()
{
#if defined(QT_OPENGL_ES_2)
    GLint count = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS_OES, &count);
    if (count <= 0)
        return QList<int>();
    QVector<int> list;
    list.resize(count);
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS_OES, list.data());
    return list.toList();
#else
    return QList<int>();
#endif
}

/*!
    Links together the shaders that were added to this program with
    addShader().  Returns true if the link was successful or
    false otherwise.  If the link failed, the error messages can
    be retrieved with log().

    Subclasses can override this function to initialize attributes
    and uniform variables for use in specific shader programs.

    If the shader program was already linked, calling this
    function again will force it to be re-linked.

    \sa addShader(), log()
*/
bool QGLShaderProgram::link()
{
    GLuint program = d->programGuard.id();
    if (!program)
        return false;
    if (d->hasPartialShaders) {
        // Compile the partial vertex and fragment shaders.
        QByteArray vertexSource;
        QByteArray fragmentSource;
        foreach (QGLShader *shader, d->shaders) {
            if (shader->shaderType() == QGLShader::PartialVertexShader)
                vertexSource += shader->sourceCode();
            else if (shader->shaderType() == QGLShader::PartialFragmentShader)
                fragmentSource += shader->sourceCode();
        }
        if (vertexSource.isEmpty()) {
            if (d->vertexShader) {
                glDetachShader(program, d->vertexShader->d->shaderGuard.id());
                delete d->vertexShader;
                d->vertexShader = 0;
            }
        } else {
            if (!d->vertexShader) {
                d->vertexShader =
                    new QGLShader(QGLShader::VertexShader, this);
            }
            if (!d->vertexShader->compile(vertexSource)) {
                d->log = d->vertexShader->log();
                return false;
            }
            glAttachShader(program, d->vertexShader->d->shaderGuard.id());
        }
        if (fragmentSource.isEmpty()) {
            if (d->fragmentShader) {
                glDetachShader(program, d->fragmentShader->d->shaderGuard.id());
                delete d->fragmentShader;
                d->fragmentShader = 0;
            }
        } else {
            if (!d->fragmentShader) {
                d->fragmentShader =
                    new QGLShader(QGLShader::FragmentShader, this);
            }
            if (!d->fragmentShader->compile(fragmentSource)) {
                d->log = d->fragmentShader->log();
                return false;
            }
            glAttachShader(program, d->fragmentShader->d->shaderGuard.id());
        }
    }
    glLinkProgram(program);
    GLint value = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &value);
    d->linked = (value != 0);
    value = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &value);
    d->log = QString();
    if (value > 1) {
        char *logbuf = new char [value];
        GLint len;
        glGetProgramInfoLog(program, value, &len, logbuf);
        d->log = QString::fromLatin1(logbuf);
        QString name = objectName();
        if (name.isEmpty())
            qWarning() << "QGLShader::link:" << d->log;
        else
            qWarning() << "QGLShader::link[" << name << "]:" << d->log;
        delete [] logbuf;
    }
    return d->linked;
}

/*!
    Returns true if this shader program has been linked; false otherwise.

    \sa link()
*/
bool QGLShaderProgram::isLinked() const
{
    return d->linked;
}

/*!
    Returns the errors and warnings that occurred during the last link()
    or addShader() with explicitly specified source code.

    \sa link()
*/
QString QGLShaderProgram::log() const
{
    return d->log;
}

/*!
    Enable use of this shader program in the currently active QGLContext.
    Returns true if the program was successfully enabled; false
    otherwise.  If the shader program has not yet been linked,
    or it needs to be re-linked, this function will call link().

    \sa link(), disable()
*/
bool QGLShaderProgram::enable()
{
    GLuint program = d->programGuard.id();
    if (!program)
        return false;
    if (!d->linked && !link())
        return false;
    glUseProgram(program);
    return true;
}

#undef ctx
#define ctx QGLContext::currentContext()

/*!
    Disables the active shader program in the current QGLContext.
    This is equivalent to calling \c{glUseProgram(0)}.

    \sa enable()
*/
void QGLShaderProgram::disable()
{
#if defined(QT_OPENGL_ES_2)
    glUseProgram(0);
#else
    if (glUseProgram)
        glUseProgram(0);
#endif
}

#undef ctx
#define ctx d->programGuard.context()

/*!
    Returns the OpenGL identifier associated with this shader program.

    \sa QGLShader::shaderId()
*/
GLuint QGLShaderProgram::programId() const
{
    return d->programGuard.id();
}

/*!
    Binds the attribute \a name to the specified \a location.  This
    function can be called before or after the program has been linked.
    Any attributes that have not been explicitly bound when the program
    is linked will be assigned locations automatically.

    \sa attributeLocation()
*/
void QGLShaderProgram::bindAttributeLocation(const char *name, int location)
{
    glBindAttribLocation(d->programGuard.id(), location, name);
}

/*!
    \overload

    Binds the attribute \a name to the specified \a location.  This
    function can be called before or after the program has been linked.
    Any attributes that have not been explicitly bound when the program
    is linked will be assigned locations automatically.

    \sa attributeLocation()
*/
void QGLShaderProgram::bindAttributeLocation(const QByteArray& name, int location)
{
    glBindAttribLocation(d->programGuard.id(), location, name.constData());
}

/*!
    \overload

    Binds the attribute \a name to the specified \a location.  This
    function can be called before or after the program has been linked.
    Any attributes that have not been explicitly bound when the program
    is linked will be assigned locations automatically.

    \sa attributeLocation()
*/
void QGLShaderProgram::bindAttributeLocation(const QString& name, int location)
{
    glBindAttribLocation(d->programGuard.id(), location, name.toLatin1().constData());
}

/*!
    Returns the location of the attribute \a name within this shader
    program's parameter list.  Returns -1 if \a name is not a valid
    attribute for this shader program.

    \sa uniformLocation(), bindAttributeLocation()
*/
int QGLShaderProgram::attributeLocation(const char *name) const
{
    if (d->linked) {
        return glGetAttribLocation(d->programGuard.id(), name);
    } else {
        qWarning() << "QGLShaderProgram::attributeLocation(" << name
                   << "): shader program is not linked";
        return -1;
    }
}

/*!
    \overload

    Returns the location of the attribute \a name within this shader
    program's parameter list.  Returns -1 if \a name is not a valid
    attribute for this shader program.

    \sa uniformLocation(), bindAttributeLocation()
*/
int QGLShaderProgram::attributeLocation(const QByteArray& name) const
{
    return attributeLocation(name.constData());
}

/*!
    \overload

    Returns the location of the attribute \a name within this shader
    program's parameter list.  Returns -1 if \a name is not a valid
    attribute for this shader program.

    \sa uniformLocation(), bindAttributeLocation()
*/
int QGLShaderProgram::attributeLocation(const QString& name) const
{
    return attributeLocation(name.toLatin1().constData());
}

/*!
    Sets the attribute at \a location in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(int location, GLfloat value)
{
    if (location != -1)
        glVertexAttrib1fv(location, &value);
}

/*!
    \overload

    Sets the attribute called \a name in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(const char *name, GLfloat value)
{
    setAttributeValue(attributeLocation(name), value);
}

/*!
    Sets the attribute at \a location in the current context to
    the 2D vector (\a x, \a y).

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(int location, GLfloat x, GLfloat y)
{
    if (location != -1) {
        GLfloat values[2] = {x, y};
        glVertexAttrib2fv(location, values);
    }
}

/*!
    \overload

    Sets the attribute called \a name in the current context to
    the 2D vector (\a x, \a y).

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(const char *name, GLfloat x, GLfloat y)
{
    setAttributeValue(attributeLocation(name), x, y);
}

/*!
    Sets the attribute at \a location in the current context to
    the 3D vector (\a x, \a y, \a z).

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue
        (int location, GLfloat x, GLfloat y, GLfloat z)
{
    if (location != -1) {
        GLfloat values[3] = {x, y, z};
        glVertexAttrib3fv(location, values);
    }
}

/*!
    \overload

    Sets the attribute called \a name in the current context to
    the 3D vector (\a x, \a y, \a z).

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue
        (const char *name, GLfloat x, GLfloat y, GLfloat z)
{
    setAttributeValue(attributeLocation(name), x, y, z);
}

/*!
    Sets the attribute at \a location in the current context to
    the 4D vector (\a x, \a y, \a z, \a w).

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue
        (int location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    if (location != -1) {
        GLfloat values[4] = {x, y, z, w};
        glVertexAttrib4fv(location, values);
    }
}

/*!
    \overload

    Sets the attribute called \a name in the current context to
    the 4D vector (\a x, \a y, \a z, \a w).

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue
        (const char *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    setAttributeValue(attributeLocation(name), x, y, z, w);
}

/*!
    Sets the attribute at \a location in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(int location, const QVector2D& value)
{
    if (location != -1)
        glVertexAttrib2fv(location, reinterpret_cast<const GLfloat *>(&value));
}

/*!
    \overload

    Sets the attribute called \a name in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(const char *name, const QVector2D& value)
{
    setAttributeValue(attributeLocation(name), value);
}

/*!
    Sets the attribute at \a location in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(int location, const QVector3D& value)
{
    if (location != -1)
        glVertexAttrib3fv(location, reinterpret_cast<const GLfloat *>(&value));
}

/*!
    \overload

    Sets the attribute called \a name in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(const char *name, const QVector3D& value)
{
    setAttributeValue(attributeLocation(name), value);
}

/*!
    Sets the attribute at \a location in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(int location, const QVector4D& value)
{
    if (location != -1)
        glVertexAttrib4fv(location, reinterpret_cast<const GLfloat *>(&value));
}

/*!
    \overload

    Sets the attribute called \a name in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(const char *name, const QVector4D& value)
{
    setAttributeValue(attributeLocation(name), value);
}

/*!
    Sets the attribute at \a location in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(int location, const QColor& value)
{
    if (location != -1) {
        GLfloat values[4] = {value.redF(), value.greenF(), value.blueF(), value.alphaF()};
        glVertexAttrib4fv(location, values);
    }
}

/*!
    \overload

    Sets the attribute called \a name in the current context to \a value.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue(const char *name, const QColor& value)
{
    setAttributeValue(attributeLocation(name), value);
}

/*!
    Sets the attribute at \a location in the current context to the
    contents of \a values, which contains \a columns elements, each
    consisting of \a rows elements.  The \a rows value should be
    1, 2, 3, or 4.  This function is typically used to set matrix
    values and column vectors.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue
    (int location, const GLfloat *values, int columns, int rows)
{
    if (rows < 1 || rows > 4) {
        qWarning() << "QGLShaderProgram::setAttributeValue: rows" << rows << "not supported";
        return;
    }
    if (location != -1) {
        while (columns-- > 0) {
            if (rows == 1)
                glVertexAttrib1fv(location, values);
            else if (rows == 2)
                glVertexAttrib2fv(location, values);
            else if (rows == 3)
                glVertexAttrib3fv(location, values);
            else
                glVertexAttrib4fv(location, values);
            values += rows;
            ++location;
        }
    }
}

/*!
    \overload

    Sets the attribute called \a name in the current context to the
    contents of \a values, which contains \a columns elements, each
    consisting of \a rows elements.  The \a rows value should be
    1, 2, 3, or 4.  This function is typically used to set matrix
    values and column vectors.

    \sa setUniformValue()
*/
void QGLShaderProgram::setAttributeValue
    (const char *name, const GLfloat *values, int columns, int rows)
{
    setAttributeValue(attributeLocation(name), values, columns, rows);
}

/*!
    Sets an array of vertex \a values on the attribute at \a location
    in this shader program.  The \a size indicates the number of
    components per vertex (1, 2, 3, or 4), and the \a stride indicates
    the number of bytes between vertices.  A default \a stride value
    of zero indicates that the vertices are densely packed in \a values.

    \sa setAttributeValue(), setUniformValue(), disableAttributeArray()
*/
void QGLShaderProgram::setAttributeArray
    (int location, const GLfloat *values, int size, int stride)
{
    if (location != -1) {
        glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE,
                              stride, values);
        glEnableVertexAttribArray(location);
    }
}

/*!
    Sets an array of 2D vertex \a values on the attribute at \a location
    in this shader program.  The \a stride indicates the number of bytes
    between vertices.  A default \a stride value of zero indicates that
    the vertices are densely packed in \a values.

    \sa setAttributeValue(), setUniformValue(), disableAttributeArray()
*/
void QGLShaderProgram::setAttributeArray
        (int location, const QVector2D *values, int stride)
{
    if (location != -1) {
        glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE,
                              stride, values);
        glEnableVertexAttribArray(location);
    }
}

/*!
    Sets an array of 3D vertex \a values on the attribute at \a location
    in this shader program.  The \a stride indicates the number of bytes
    between vertices.  A default \a stride value of zero indicates that
    the vertices are densely packed in \a values.

    \sa setAttributeValue(), setUniformValue(), disableAttributeArray()
*/
void QGLShaderProgram::setAttributeArray
        (int location, const QVector3D *values, int stride)
{
    if (location != -1) {
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE,
                              stride, values);
        glEnableVertexAttribArray(location);
    }
}

/*!
    Sets an array of 4D vertex \a values on the attribute at \a location
    in this shader program.  The \a stride indicates the number of bytes
    between vertices.  A default \a stride value of zero indicates that
    the vertices are densely packed in \a values.

    \sa setAttributeValue(), setUniformValue(), disableAttributeArray()
*/
void QGLShaderProgram::setAttributeArray
        (int location, const QVector4D *values, int stride)
{
    if (location != -1) {
        glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE,
                              stride, values);
        glEnableVertexAttribArray(location);
    }
}

/*!
    \overload

    Sets an array of vertex \a values on the attribute called \a name
    in this shader program.  The \a size indicates the number of
    components per vertex (1, 2, 3, or 4), and the \a stride indicates
    the number of bytes between vertices.  A default \a stride value
    of zero indicates that the vertices are densely packed in \a values.

    \sa setAttributeValue(), setUniformValue(), disableAttributeArray()
*/
void QGLShaderProgram::setAttributeArray
    (const char *name, const GLfloat *values, int size, int stride)
{
    setAttributeArray(attributeLocation(name), values, size, stride);
}

/*!
    \overload

    Sets an array of 2D vertex \a values on the attribute called \a name
    in this shader program.  The \a stride indicates the number of bytes
    between vertices.  A default \a stride value of zero indicates that
    the vertices are densely packed in \a values.

    \sa setAttributeValue(), setUniformValue(), disableAttributeArray()
*/
void QGLShaderProgram::setAttributeArray
        (const char *name, const QVector2D *values, int stride)
{
    setAttributeArray(attributeLocation(name), values, stride);
}

/*!
    \overload

    Sets an array of 3D vertex \a values on the attribute called \a name
    in this shader program.  The \a stride indicates the number of bytes
    between vertices.  A default \a stride value of zero indicates that
    the vertices are densely packed in \a values.

    \sa setAttributeValue(), setUniformValue(), disableAttributeArray()
*/
void QGLShaderProgram::setAttributeArray
        (const char *name, const QVector3D *values, int stride)
{
    setAttributeArray(attributeLocation(name), values, stride);
}

/*!
    \overload

    Sets an array of 4D vertex \a values on the attribute called \a name
    in this shader program.  The \a stride indicates the number of bytes
    between vertices.  A default \a stride value of zero indicates that
    the vertices are densely packed in \a values.

    \sa setAttributeValue(), setUniformValue(), disableAttributeArray()
*/
void QGLShaderProgram::setAttributeArray
        (const char *name, const QVector4D *values, int stride)
{
    setAttributeArray(attributeLocation(name), values, stride);
}

/*!
    Disables the vertex array at \a location in this shader program
    that was enabled by a previous call to setAttributeArray().

    \sa setAttributeArray(), setAttributeValue(), setUniformValue()
*/
void QGLShaderProgram::disableAttributeArray(int location)
{
    if (location != -1)
        glDisableVertexAttribArray(location);
}

/*!
    \overload

    Disables the vertex array called \a name in this shader program
    that was enabled by a previous call to setAttributeArray().

    \sa setAttributeArray(), setAttributeValue(), setUniformValue()
*/
void QGLShaderProgram::disableAttributeArray(const char *name)
{
    disableAttributeArray(attributeLocation(name));
}

/*!
    Returns the location of the uniform variable \a name within this shader
    program's parameter list.  Returns -1 if \a name is not a valid
    uniform variable for this shader program.

    \sa attributeLocation()
*/
int QGLShaderProgram::uniformLocation(const char *name) const
{
    if (d->linked) {
        return glGetUniformLocation(d->programGuard.id(), name);
    } else {
        qWarning() << "QGLShaderProgram::uniformLocation(" << name
                   << "): shader program is not linked";
        return -1;
    }
}

/*!
    \overload

    Returns the location of the uniform variable \a name within this shader
    program's parameter list.  Returns -1 if \a name is not a valid
    uniform variable for this shader program.

    \sa attributeLocation()
*/
int QGLShaderProgram::uniformLocation(const QByteArray& name) const
{
    return uniformLocation(name.constData());
}

/*!
    \overload

    Returns the location of the uniform variable \a name within this shader
    program's parameter list.  Returns -1 if \a name is not a valid
    uniform variable for this shader program.

    \sa attributeLocation()
*/
int QGLShaderProgram::uniformLocation(const QString& name) const
{
    return uniformLocation(name.toLatin1().constData());
}

/*!
    Sets the uniform variable at \a location in the current context to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, GLfloat value)
{
    if (location != -1)
        glUniform1fv(location, 1, &value);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, GLfloat value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, GLint value)
{
    if (location != -1)
        glUniform1i(location, value);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, GLint value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context to \a value.
    This function should be used when setting sampler values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, GLuint value)
{
    if (location != -1)
        glUniform1i(location, value);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to \a value.  This function should be used when setting sampler values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, GLuint value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context to
    the 2D vector (\a x, \a y).

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, GLfloat x, GLfloat y)
{
    if (location != -1) {
        GLfloat values[2] = {x, y};
        glUniform2fv(location, 1, values);
    }
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context to
    the 2D vector (\a x, \a y).

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, GLfloat x, GLfloat y)
{
    setUniformValue(uniformLocation(name), x, y);
}

/*!
    Sets the uniform variable at \a location in the current context to
    the 3D vector (\a x, \a y, \a z).

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue
        (int location, GLfloat x, GLfloat y, GLfloat z)
{
    if (location != -1) {
        GLfloat values[3] = {x, y, z};
        glUniform3fv(location, 1, values);
    }
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context to
    the 3D vector (\a x, \a y, \a z).

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue
        (const char *name, GLfloat x, GLfloat y, GLfloat z)
{
    setUniformValue(uniformLocation(name), x, y, z);
}

/*!
    Sets the uniform variable at \a location in the current context to
    the 4D vector (\a x, \a y, \a z, \a w).

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue
        (int location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    if (location != -1) {
        GLfloat values[4] = {x, y, z, w};
        glUniform4fv(location, 1, values);
    }
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context to
    the 4D vector (\a x, \a y, \a z, \a w).

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue
        (const char *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    setUniformValue(uniformLocation(name), x, y, z, w);
}

/*!
    Sets the uniform variable at \a location in the current context to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QVector2D& value)
{
    if (location != -1)
        glUniform2fv(location, 1, reinterpret_cast<const GLfloat *>(&value));
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QVector2D& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QVector3D& value)
{
    if (location != -1)
        glUniform3fv(location, 1, reinterpret_cast<const GLfloat *>(&value));
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QVector3D& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QVector4D& value)
{
    if (location != -1)
        glUniform4fv(location, 1, reinterpret_cast<const GLfloat *>(&value));
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QVector4D& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context to
    the red, green, blue, and alpha components of \a color.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QColor& color)
{
    if (location != -1) {
        GLfloat values[4] = {color.redF(), color.greenF(), color.blueF(), color.alphaF()};
        glUniform4fv(location, 1, values);
    }
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context to
    the red, green, blue, and alpha components of \a color.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QColor& color)
{
    setUniformValue(uniformLocation(name), color);
}

/*!
    Sets the uniform variable at \a location in the current context to
    the x and y coordinates of \a point.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QPoint& point)
{
    if (location != -1) {
        GLfloat values[4] = {point.x(), point.y()};
        glUniform2fv(location, 1, values);
    }
}

/*!
    \overload

    Sets the uniform variable associated with \a name in the current
    context to the x and y coordinates of \a point.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QPoint& point)
{
    setUniformValue(uniformLocation(name), point);
}

/*!
    Sets the uniform variable at \a location in the current context to
    the x and y coordinates of \a point.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QPointF& point)
{
    if (location != -1) {
        GLfloat values[4] = {point.x(), point.y()};
        glUniform2fv(location, 1, values);
    }
}

/*!
    \overload

    Sets the uniform variable associated with \a name in the current
    context to the x and y coordinates of \a point.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QPointF& point)
{
    setUniformValue(uniformLocation(name), point);
}

/*!
    Sets the uniform variable at \a location in the current context to
    the width and height of the given \a size.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QSize& size)
{
    if (location != -1) {
        GLfloat values[4] = {size.width(), size.width()};
        glUniform2fv(location, 1, values);
    }
}

/*!
    \overload

    Sets the uniform variable associated with \a name in the current
    context to the width and height of the given \a size.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QSize& size)
{
    setUniformValue(uniformLocation(name), size);
}

/*!
    Sets the uniform variable at \a location in the current context to
    the width and height of the given \a size.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QSizeF& size)
{
    if (location != -1) {
        GLfloat values[4] = {size.width(), size.height()};
        glUniform2fv(location, 1, values);
    }
}

/*!
    \overload

    Sets the uniform variable associated with \a name in the current
    context to the width and height of the given \a size.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QSizeF& size)
{
    setUniformValue(uniformLocation(name), size);
}

// We have to repack matrices from qreal to GLfloat.
#define setUniformMatrix(func,location,value,cols,rows) \
    if (location == -1) \
        return; \
    if (sizeof(qreal) == sizeof(GLfloat)) { \
        func(location, 1, GL_FALSE, \
             reinterpret_cast<const GLfloat *>(value.constData())); \
    } else { \
        GLfloat mat[cols * rows]; \
        const qreal *data = value.constData(); \
        for (int i = 0; i < cols * rows; ++i) \
            mat[i] = data[i]; \
        func(location, 1, GL_FALSE, mat); \
    }
#if !defined(QT_OPENGL_ES_2)
#define setUniformGenericMatrix(func,colfunc,location,value,cols,rows) \
    if (location == -1) \
        return; \
    if (sizeof(qreal) == sizeof(GLfloat)) { \
        const GLfloat *data = reinterpret_cast<const GLfloat *> \
            (value.constData());  \
        if (func) \
            func(location, 1, GL_FALSE, data); \
        else \
            colfunc(location, cols, data); \
    } else { \
        GLfloat mat[cols * rows]; \
        const qreal *data = value.constData(); \
        for (int i = 0; i < cols * rows; ++i) \
            mat[i] = data[i]; \
        if (func) \
            func(location, 1, GL_FALSE, mat); \
        else \
            colfunc(location, cols, mat); \
    }
#else
#define setUniformGenericMatrix(func,colfunc,location,value,cols,rows) \
    if (location == -1) \
        return; \
    if (sizeof(qreal) == sizeof(GLfloat)) { \
        const GLfloat *data = reinterpret_cast<const GLfloat *> \
            (value.constData());  \
        colfunc(location, cols, data); \
    } else { \
        GLfloat mat[cols * rows]; \
        const qreal *data = value.constData(); \
        for (int i = 0; i < cols * rows; ++i) \
            mat[i] = data[i]; \
        colfunc(location, cols, mat); \
    }
#endif

/*!
    Sets the uniform variable at \a location in the current context
    to a 2x2 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix2x2& value)
{
    setUniformMatrix(glUniformMatrix2fv, location, value, 2, 2);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 2x2 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix2x2& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context
    to a 2x3 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix2x3& value)
{
    setUniformGenericMatrix
        (glUniformMatrix2x3fv, glUniform3fv, location, value, 2, 3);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 2x3 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix2x3& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context
    to a 2x4 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix2x4& value)
{
    setUniformGenericMatrix
        (glUniformMatrix2x4fv, glUniform4fv, location, value, 2, 4);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 2x4 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix2x4& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context
    to a 3x2 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix3x2& value)
{
    setUniformGenericMatrix
        (glUniformMatrix3x2fv, glUniform2fv, location, value, 3, 2);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 3x2 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix3x2& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context
    to a 3x3 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix3x3& value)
{
    setUniformMatrix(glUniformMatrix3fv, location, value, 3, 3);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 3x3 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix3x3& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context
    to a 3x4 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix3x4& value)
{
    setUniformGenericMatrix
        (glUniformMatrix3x4fv, glUniform4fv, location, value, 3, 4);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 3x4 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix3x4& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context
    to a 4x2 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix4x2& value)
{
    setUniformGenericMatrix
        (glUniformMatrix4x2fv, glUniform2fv, location, value, 4, 2);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 4x2 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix4x2& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context
    to a 4x3 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix4x3& value)
{
    setUniformGenericMatrix
        (glUniformMatrix4x3fv, glUniform3fv, location, value, 4, 3);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 4x3 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix4x3& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context
    to a 4x4 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const QMatrix4x4& value)
{
    setUniformMatrix(glUniformMatrix4fv, location, value, 4, 4);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 4x4 matrix \a value.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const QMatrix4x4& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    \overload

    Sets the uniform variable at \a location in the current context
    to a 4x4 matrix \a value.  The matrix elements must be specified
    in column-major order.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(int location, const GLfloat value[4][4])
{
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, value[0]);
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context
    to a 4x4 matrix \a value.  The matrix elements must be specified
    in column-major order.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValue(const char *name, const GLfloat value[4][4])
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable at \a location in the current context to a
    3x3 transformation matrix \a value that is specified as a QTransform value.

    To set a QTransform value as a 4x4 matrix in a shader, use
    \c{setUniformValue(location, QMatrix4x4(value))}.
*/
void QGLShaderProgram::setUniformValue(int location, const QTransform& value)
{
    if (location != -1) {
        GLfloat mat[3][3] = {
            {value.m11(), value.m12(), value.m13()},
            {value.m21(), value.m22(), value.m23()},
            {value.m31(), value.m32(), value.m33()}
        };
        glUniformMatrix3fv(location, 1, GL_FALSE, mat[0]);
    }
}

/*!
    \overload

    Sets the uniform variable called \a name in the current context to a
    3x3 transformation matrix \a value that is specified as a QTransform value.

    To set a QTransform value as a 4x4 matrix in a shader, use
    \c{setUniformValue(name, QMatrix4x4(value))}.
*/
void QGLShaderProgram::setUniformValue
        (const char *name, const QTransform& value)
{
    setUniformValue(uniformLocation(name), value);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const GLint *values, int count)
{
    if (location != -1)
        glUniform1iv(location, count, values);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray
        (const char *name, const GLint *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count elements of \a values.  This overload
    should be used when setting an array of sampler values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const GLuint *values, int count)
{
    if (location != -1)
        glUniform1iv(location, count, reinterpret_cast<const GLint *>(values));
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count elements of \a values.  This overload
    should be used when setting an array of sampler values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray
        (const char *name, const GLuint *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count elements of \a values.  Each element
    has \a size components.  The \a size must be 1, 2, 3, or 4.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const GLfloat *values, int count, int size)
{
    if (location != -1) {
        if (size == 1)
            glUniform1fv(location, count, values);
        else if (size == 2)
            glUniform2fv(location, count, values);
        else if (size == 3)
            glUniform3fv(location, count, values);
        else if (size == 4)
            glUniform4fv(location, count, values);
        else
            qWarning() << "QGLShaderProgram::setUniformValue: size" << size << "not supported";
    }
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count elements of \a values.  Each element
    has \a size components.  The \a size must be 1, 2, 3, or 4.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray
        (const char *name, const GLfloat *values, int count, int size)
{
    setUniformValueArray(uniformLocation(name), values, count, size);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 2D vector elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QVector2D *values, int count)
{
    if (location != -1)
        glUniform2fv(location, count, reinterpret_cast<const GLfloat *>(values));
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 2D vector elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QVector2D *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 3D vector elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QVector3D *values, int count)
{
    if (location != -1)
        glUniform3fv(location, count, reinterpret_cast<const GLfloat *>(values));
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 3D vector elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QVector3D *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 4D vector elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QVector4D *values, int count)
{
    if (location != -1)
        glUniform4fv(location, count, reinterpret_cast<const GLfloat *>(values));
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 4D vector elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QVector4D *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

// We have to repack matrix arrays from qreal to GLfloat.
#define setUniformMatrixArray(func,location,values,count,type,cols,rows) \
    if (location == -1 || count <= 0) \
        return; \
    if (sizeof(type) == sizeof(GLfloat) * cols * rows) { \
        func(location, count, GL_FALSE, \
             reinterpret_cast<const GLfloat *>(values[0].constData())); \
    } else { \
        QVarLengthArray<GLfloat> temp(cols * rows * count); \
        for (int index = 0; index < count; ++index) { \
            for (int index2 = 0; index2 < (cols * rows); ++index2) { \
                temp.data()[cols * rows * index + index2] = \
                    values[index].constData()[index2]; \
            } \
        } \
        func(location, count, GL_FALSE, temp.constData()); \
    }
#if !defined(QT_OPENGL_ES_2)
#define setUniformGenericMatrixArray(func,colfunc,location,values,count,type,cols,rows) \
    if (location == -1 || count <= 0) \
        return; \
    if (sizeof(type) == sizeof(GLfloat) * cols * rows) { \
        const GLfloat *data = reinterpret_cast<const GLfloat *> \
            (values[0].constData());  \
        if (func) \
            func(location, count, GL_FALSE, data); \
        else \
            colfunc(location, count * cols, data); \
    } else { \
        QVarLengthArray<GLfloat> temp(cols * rows * count); \
        for (int index = 0; index < count; ++index) { \
            for (int index2 = 0; index2 < (cols * rows); ++index2) { \
                temp.data()[cols * rows * index + index2] = \
                    values[index].constData()[index2]; \
            } \
        } \
        if (func) \
            func(location, count, GL_FALSE, temp.constData()); \
        else \
            colfunc(location, count * cols, temp.constData()); \
    }
#else
#define setUniformGenericMatrixArray(func,colfunc,location,values,count,type,cols,rows) \
    if (location == -1 || count <= 0) \
        return; \
    if (sizeof(type) == sizeof(GLfloat) * cols * rows) { \
        const GLfloat *data = reinterpret_cast<const GLfloat *> \
            (values[0].constData());  \
        colfunc(location, count * cols, data); \
    } else { \
        QVarLengthArray<GLfloat> temp(cols * rows * count); \
        for (int index = 0; index < count; ++index) { \
            for (int index2 = 0; index2 < (cols * rows); ++index2) { \
                temp.data()[cols * rows * index + index2] = \
                    values[index].constData()[index2]; \
            } \
        } \
        colfunc(location, count * cols, temp.constData()); \
    }
#endif

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 2x2 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix2x2 *values, int count)
{
    setUniformMatrixArray
        (glUniformMatrix2fv, location, values, count, QMatrix2x2, 2, 2);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 2x2 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix2x2 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 2x3 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix2x3 *values, int count)
{
    setUniformGenericMatrixArray
        (glUniformMatrix2x3fv, glUniform3fv, location, values, count,
         QMatrix2x3, 2, 3);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 2x3 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix2x3 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 2x4 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix2x4 *values, int count)
{
    setUniformGenericMatrixArray
        (glUniformMatrix2x4fv, glUniform4fv, location, values, count,
         QMatrix2x4, 2, 4);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 2x4 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix2x4 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 3x2 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix3x2 *values, int count)
{
    setUniformGenericMatrixArray
        (glUniformMatrix3x2fv, glUniform2fv, location, values, count,
         QMatrix3x2, 3, 2);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 3x2 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix3x2 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 3x3 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix3x3 *values, int count)
{
    setUniformMatrixArray
        (glUniformMatrix3fv, location, values, count, QMatrix3x3, 3, 3);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 3x3 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix3x3 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 3x4 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix3x4 *values, int count)
{
    setUniformGenericMatrixArray
        (glUniformMatrix3x4fv, glUniform4fv, location, values, count,
         QMatrix3x4, 3, 4);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 3x4 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix3x4 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 4x2 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix4x2 *values, int count)
{
    setUniformGenericMatrixArray
        (glUniformMatrix4x2fv, glUniform2fv, location, values, count,
         QMatrix4x2, 4, 2);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 4x2 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix4x2 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 4x3 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix4x3 *values, int count)
{
    setUniformGenericMatrixArray
        (glUniformMatrix4x3fv, glUniform3fv, location, values, count,
         QMatrix4x3, 4, 3);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 4x3 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix4x3 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

/*!
    Sets the uniform variable array at \a location in the current
    context to the \a count 4x4 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(int location, const QMatrix4x4 *values, int count)
{
    setUniformMatrixArray
        (glUniformMatrix4fv, location, values, count, QMatrix4x4, 4, 4);
}

/*!
    \overload

    Sets the uniform variable array called \a name in the current
    context to the \a count 4x4 matrix elements of \a values.

    \sa setAttributeValue()
*/
void QGLShaderProgram::setUniformValueArray(const char *name, const QMatrix4x4 *values, int count)
{
    setUniformValueArray(uniformLocation(name), values, count);
}

#undef ctx

/*!
    Returns true if shader programs written in the OpenGL Shading
    Language (GLSL) are supported on this system; false otherwise.

    The \a context is used to resolve the GLSL extensions.
    If \a context is null, then QGLContext::currentContext() is used.
*/
bool QGLShaderProgram::hasShaderPrograms(const QGLContext *context)
{
#if !defined(QT_OPENGL_ES_2)
    if (!context)
        context = QGLContext::currentContext();
    if (!context)
        return false;
    return qt_resolve_glsl_extensions(const_cast<QGLContext *>(context));
#else
    Q_UNUSED(context);
    return true;
#endif
}

/*!
    \internal
*/
void QGLShaderProgram::shaderDestroyed()
{
    QGLShader *shader = qobject_cast<QGLShader *>(sender());
    if (shader && !d->removingShaders)
        removeShader(shader);
}

#endif

QT_END_NAMESPACE
