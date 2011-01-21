/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <qglnamespace.h>
#include "renderer.h"
#include <qglshaderprogram.h>

class AbstractEffect;

class Q_DECLARATIVE_EXPORT AbstractEffectProgram
{
public:
    virtual ~AbstractEffectProgram() { }

    virtual void activate() { }
    virtual void deactivate() { }
    virtual void updateRendererState(Renderer *renderer, Renderer::Updates updates) { Q_UNUSED(renderer) Q_UNUSED(updates) }
    virtual void updateEffectState(Renderer *renderer, AbstractEffect *newEffect, AbstractEffect *oldEffect) { Q_UNUSED(renderer) Q_UNUSED(newEffect) Q_UNUSED(oldEffect) }
    virtual const QGL::VertexAttribute *requiredFields() const = 0; // Array must end with QGL::VertexAttribute(-1).
};

class Q_DECLARATIVE_EXPORT AbstractShaderEffectProgram : public AbstractEffectProgram
{
public:
    AbstractShaderEffectProgram();

    virtual void activate();
    virtual void deactivate();
    virtual const QGL::VertexAttribute *requiredFields() const;
protected:
    struct Attributes
    {
        const QGL::VertexAttribute *ids;
        const char *const *names;
    };

    virtual void initialize();

    virtual const char *vertexShader() const = 0;
    virtual const char *fragmentShader() const = 0;
    virtual const Attributes attributes() const = 0;

    QGLShaderProgram m_program;
    bool m_initialized;
};


struct AbstractEffectType { };

class Q_DECLARATIVE_EXPORT AbstractEffect
{
public:
    AbstractEffect();
    virtual ~AbstractEffect();

    enum Flag {
        Blending        = 0x0001,
        SupportsPicking = 0x0002
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    virtual AbstractEffectType *type() const = 0;
    virtual AbstractEffectProgram *createProgram() const = 0;
    virtual int compare(const AbstractEffect *other) const { return this - other; }

    AbstractEffect::Flags flags() const { return m_flags; }

protected:
    //void updated();

    void setFlags(Flags flags) { m_flags = flags; }

private:
    Flags m_flags;

    Q_DISABLE_COPY(AbstractEffect)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractEffect::Flags);




#endif
