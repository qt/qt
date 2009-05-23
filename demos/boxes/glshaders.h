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
