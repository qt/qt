/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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

#include "glshaders.h"

#define GLSHADERS_ASSERT_OPENGL(prefix, assertion, returnStatement)                         \
if (m_failed || !(assertion)) {                                                             \
    if (!m_failed) qCritical(prefix ": The necessary OpenGL functions are not available."); \
    m_failed = true;                                                                        \
    returnStatement;                                                                        \
}


GLShader::GLShader(const char *data, int size, GLenum shaderType)
: m_compileError(false), m_failed(false)
{
    GLSHADERS_ASSERT_OPENGL("GLShader::GLShader",
        glCreateShaderObjectARB && glShaderSourceARB && glCompileShaderARB && glGetObjectParameterivARB, return)

    m_shader = glCreateShaderObjectARB(shaderType);

    GLint glSize = size;
    glShaderSourceARB(m_shader, 1, &data, &glSize);
    glCompileShaderARB(m_shader);
    int status;
    glGetObjectParameterivARB(m_shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
    m_compileError = (status != 1);
}

GLShader::GLShader(const QString& fileName, GLenum shaderType)
    : m_compileError(false), m_failed(false)
{
    GLSHADERS_ASSERT_OPENGL("GLShader::GLShader",
        glCreateShaderObjectARB && glShaderSourceARB && glCompileShaderARB && glGetObjectParameterivARB, return)

    m_shader = glCreateShaderObjectARB(shaderType);

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray bytes = file.readAll();
        GLint size = file.size();
        const char *p = bytes.data();
        file.close();
        glShaderSourceARB(m_shader, 1, &p, &size);
        glCompileShaderARB(m_shader);
        int status;
        glGetObjectParameterivARB(m_shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
        m_compileError = (status != 1);
    } else {
        m_compileError = true;
    }
}

GLShader::~GLShader()
{
    GLSHADERS_ASSERT_OPENGL("GLShader::~GLShader", glDeleteObjectARB, return)

    glDeleteObjectARB(m_shader);
}

QString GLShader::log()
{
    GLSHADERS_ASSERT_OPENGL("GLShader::log", glGetObjectParameterivARB
        && glGetInfoLogARB, return QLatin1String("GLSL not supported."))

    int length;
    glGetObjectParameterivARB(m_shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    char *log = new char[length + 1];
    GLsizei glLength = length;
    glGetInfoLogARB(m_shader, glLength, &glLength, log);
    log[glLength] = '\0';
    QString result(log);
    delete log;
    return result;
}

GLVertexShader::GLVertexShader(const char *data, int size) : GLShader(data, size, GL_VERTEX_SHADER_ARB)
{
}

GLVertexShader::GLVertexShader(const QString& fileName) : GLShader(fileName, GL_VERTEX_SHADER_ARB)
{
}

GLFragmentShader::GLFragmentShader(const char *data, int size) : GLShader(data, size, GL_FRAGMENT_SHADER_ARB)
{
}

GLFragmentShader::GLFragmentShader(const QString& fileName) : GLShader(fileName, GL_FRAGMENT_SHADER_ARB)
{
}

GLProgram::GLProgram() : m_linked(false), m_linkError(false), m_failed(false)
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::GLProgram", glCreateProgramObjectARB, return)

    m_program = glCreateProgramObjectARB();
}

GLProgram::~GLProgram()
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::~GLProgram", glDeleteObjectARB, return)

    glDeleteObjectARB(m_program);
}

void GLProgram::attach(const GLShader &shader)
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::attach", glAttachObjectARB, return)

    glAttachObjectARB(m_program, shader.m_shader);
    m_linked = m_linkError = false;
}

void GLProgram::detach(const GLShader &shader)
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::detach", glDetachObjectARB, return)

    glDetachObjectARB(m_program, shader.m_shader);
    m_linked = m_linkError = false;
}

bool GLProgram::failed()
{
    if (m_failed || m_linkError)
        return true;

    if (m_linked)
        return false;

    GLSHADERS_ASSERT_OPENGL("GLProgram::failed", glLinkProgramARB && glGetObjectParameterivARB, return true)

    glLinkProgramARB(m_program);
    int status;
    glGetObjectParameterivARB(m_program, GL_OBJECT_LINK_STATUS_ARB, &status);
    m_linkError = !(m_linked = (status == 1));
    return m_linkError;
}

QString GLProgram::log()
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::log", glGetObjectParameterivARB && glGetInfoLogARB,
        return QLatin1String("Failed."))

    int length;
    glGetObjectParameterivARB(m_program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    char *log = new char[length + 1];
    GLsizei glLength = length;
    glGetInfoLogARB(m_program, glLength, &glLength, log);
    log[glLength] = '\0';
    QString result(log);
    delete log;
    return result;
}

void GLProgram::bind()
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::bind", glUseProgramObjectARB, return)

    if (!failed())
        glUseProgramObjectARB(m_program);
}

void GLProgram::unbind()
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::bind", glUseProgramObjectARB, return)

    glUseProgramObjectARB(0);
}

bool GLProgram::hasParameter(const QString& name)
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::hasParameter", glGetUniformLocationARB, return false)

    if (!failed()) {
        QByteArray asciiName = name.toAscii();
        return -1 != glGetUniformLocationARB(m_program, asciiName.data());
    }
    return false;
}

void GLProgram::setInt(const QString& name, int value)
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::setInt", glGetUniformLocationARB && glUniform1iARB, return)

    if (!failed()) {
        QByteArray asciiName = name.toAscii();
        int loc = glGetUniformLocationARB(m_program, asciiName.data());
	    glUniform1iARB(loc, value);
    }
}

void GLProgram::setFloat(const QString& name, float value)
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::setFloat", glGetUniformLocationARB && glUniform1fARB, return)

    if (!failed()) {
        QByteArray asciiName = name.toAscii();
        int loc = glGetUniformLocationARB(m_program, asciiName.data());
	    glUniform1fARB(loc, value);
    }
}

void GLProgram::setColor(const QString& name, QRgb value)
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::setColor", glGetUniformLocationARB && glUniform4fARB, return)

    //qDebug() << "Setting color" << name;
    if (!failed()) {
        QByteArray asciiName = name.toAscii();
        int loc = glGetUniformLocationARB(m_program, asciiName.data());
        //qDebug() << "Location of" << name << "is" << loc;
        QColor color(value);
	    glUniform4fARB(loc, color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
}

void GLProgram::setMatrix(const QString& name, const gfx::Matrix4x4f &mat)
{
    GLSHADERS_ASSERT_OPENGL("GLProgram::setMatrix", glGetUniformLocationARB && glUniformMatrix4fvARB, return)

    if (!failed()) {
        QByteArray asciiName = name.toAscii();
        int loc = glGetUniformLocationARB(m_program, asciiName.data());
        //qDebug() << "Location of" << name << "is" << loc;
	    glUniformMatrix4fvARB(loc, 1, GL_FALSE, mat.bits());
    }
}