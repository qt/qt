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

#ifndef _GLBASICSHADERS_H_
#define _GLBASICSHADERS_H_

#include <QtOpenGL/qglshaderprogram.h>
#include <QtGui/qmatrix4x4.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class BlurTextureShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    BlurTextureShader();

    enum { Vertices = 0, 
           TextureCoords = 1 };

    enum Mode { Horizontal, Vertical };
    void setMode(Mode);
    void setStep(float);
    void setSteps(int);
    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint mode;
    GLint step;
    GLint steps;
    GLint transform;
};

class SingleTextureShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    SingleTextureShader();

    enum { Vertices = 0, 
           TextureCoords = 1 };

    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint transform;
};

class DualTextureBlendShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    DualTextureBlendShader();
    enum { Vertices = 0, 
           TextureCoords = 1,
           BlendTextureCoords = 2 };

    void setOpacity(GLfloat);
    void setBlend(GLfloat);
    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint transform;
    GLint opacity;
    GLint blend;
};

class DualTextureAddShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    DualTextureAddShader();
    enum { Vertices = 0, 
           TextureCoords = 1,
           AddTextureCoords = 2 };

    void setOpacity(GLfloat);
    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint transform;
    GLint opacity;
};

class SingleTextureOpacityShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    SingleTextureOpacityShader();

    enum { Vertices = 0, 
           TextureCoords = 1 };

    void setOpacity(GLfloat);
    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint transform;
    GLint opacity;
};

class SingleTextureVertexOpacityShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    SingleTextureVertexOpacityShader();

    enum { Vertices = 0, 
           TextureCoords = 1,
           OpacityCoords = 2 };

    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint transform;
};


class SingleTextureShadowShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    SingleTextureShadowShader();

    enum { Vertices = 0, 
           TextureCoords = 1 };

    void setOpacity(GLfloat);
    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint transform;
    GLint opacity;
};


class QColor;
class ConstantColorShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    ConstantColorShader();

    enum { Vertices = 0 };

    void setColor(const QColor &);
    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint transform;
    GLint color;
};

class ColorShader : public QGLShaderProgram
{
    Q_OBJECT
public:
    ColorShader();

    enum { Vertices = 0, Colors = 1 };

    void setTransform(const QMatrix4x4 &);
    virtual bool link();

private:
    GLint transform;
};

class GLBasicShadersPrivate;
class GLBasicShaders
{
public:
    GLBasicShaders();
    virtual ~GLBasicShaders();

    BlurTextureShader *blurTexture();
    SingleTextureShader *singleTexture();
    SingleTextureOpacityShader *singleTextureOpacity();
    DualTextureAddShader *dualTextureAdd();
    SingleTextureVertexOpacityShader *singleTextureVertexOpacity();
    SingleTextureShadowShader *singleTextureShadow();
    ConstantColorShader *constantColor();
    ColorShader *color();

private:
    GLBasicShadersPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // _GLBASICSHADERS_H_
