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

#include <qglshaderprogram.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGMaterial;

class Q_DECLARATIVE_EXPORT QSGMaterialShader
{
public:
    class Q_DECLARATIVE_EXPORT RenderState {
    public:
        enum DirtyState
        {
            DirtyMatrix         = 0x0001,
            DirtyOpacity        = 0x0002
        };
        Q_DECLARE_FLAGS(DirtyStates, DirtyState)

        inline DirtyStates dirtyState() const { return m_dirty; }

        inline bool isMatrixDirty() const { return m_dirty & DirtyMatrix; }
        inline bool isOpacityDirty() const { return m_dirty & DirtyOpacity; }

        float opacity() const;
        QMatrix4x4 combinedMatrix() const;
        QMatrix4x4 modelViewMatrix() const;
        QRect viewportRect() const;
        QRect deviceRect() const;

        const QGLContext *context() const;

    private:
        friend class QSGRenderer;
        DirtyStates m_dirty;
        const void *m_data;
    };

    QSGMaterialShader();

    virtual void activate();
    virtual void deactivate();
    // First time a material is used, oldMaterial is null.
    virtual void updateState(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial);
    virtual char const *const *attributeNames() const = 0; // Array must end with null.

protected:
    void compile();
    virtual void initialize() { }

    virtual const char *vertexShader() const = 0;
    virtual const char *fragmentShader() const = 0;

    QGLShaderProgram m_program;
    bool m_compiled;
    void *m_reserved;
};

struct QSGMaterialType { };

class Q_DECLARATIVE_EXPORT QSGMaterial
{
public:
    enum Flag {
        Blending = 0x0001
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    QSGMaterial();
    virtual ~QSGMaterial();

    virtual QSGMaterialType *type() const = 0;
    virtual QSGMaterialShader *createShader() const = 0;
    virtual int compare(const QSGMaterial *other) const;

    QSGMaterial::Flags flags() const { return m_flags; }

protected:
    void setFlag(Flags flags, bool set);

private:
    Flags m_flags;
    void *m_reserved;
    Q_DISABLE_COPY(QSGMaterial)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSGMaterial::Flags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QSGMaterialShader::RenderState::DirtyStates)

QT_END_NAMESPACE

QT_END_HEADER

#endif
