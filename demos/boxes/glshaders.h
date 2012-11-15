/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GLSHADERS_H
#define GLSHADERS_H

//#include <GL/glew.h>
#include "glextensions.h"

#include <QtGui>
#include <QtOpenGL>

#include "vector.h"

class GLShader
{
public:
    friend class GLProgram;
    virtual ~GLShader();
    bool failed() const {return m_failed;}
    QString log();
protected:
    GLShader(const char *data, int size, GLenum shaderType);
    GLShader(const QString& fileName, GLenum shaderType);

    GLhandleARB m_shader;
    bool m_compileError;
    bool m_failed;
};

class GLVertexShader : public GLShader
{
public:
    GLVertexShader(const char *data, int size);
    GLVertexShader(const QString& fileName);
};

class GLFragmentShader : public GLShader
{
public:
    GLFragmentShader(const char *data, int size);
    GLFragmentShader(const QString& fileName);
};

class GLProgram
{
public:
    GLProgram();
    ~GLProgram();
    void attach(const GLShader &shader);
    void detach(const GLShader &shader);
    void bind();
    void unbind();
    bool failed();
    QString log();
    bool hasParameter(const QString& name);
    // use program before setting values
    void setInt(const QString& name, int value);
    void setFloat(const QString& name, float value);
    void setColor(const QString& name, QRgb value);
    void setMatrix(const QString& name, const gfx::Matrix4x4f &mat);
    // TODO: add a bunch of set-functions for different types.
private:
    GLhandleARB m_program;
    bool m_linked;
    bool m_linkError;
    bool m_failed;
};

#endif
