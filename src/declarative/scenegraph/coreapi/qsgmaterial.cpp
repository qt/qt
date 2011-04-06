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

#include "qsgmaterial.h"
#include "qsgrenderer_p.h"

QT_BEGIN_NAMESPACE

QSGMaterialShader::QSGMaterialShader()
    : m_compiled(false)
{
}

void QSGMaterialShader::activate()
{
    if (!m_compiled)
        compile();

    m_program.bind();
    char const *const *attr = attributeNames();
    for (int i = 0; attr[i]; ++i) {
        if (*attr[i])
            m_program.enableAttributeArray(i);
    }
}

void QSGMaterialShader::deactivate()
{
    char const *const *attr = attributeNames();
    for (int i = 0; attr[i]; ++i) {
        if (*attr[i])
            m_program.disableAttributeArray(i);
    }
}

void QSGMaterialShader::updateState(const RenderState &, QSGMaterial *, QSGMaterial *)
{
}

void QSGMaterialShader::compile()
{
    Q_ASSERT(!m_compiled);

    m_program.addShaderFromSourceCode(QGLShader::Vertex, vertexShader());
    m_program.addShaderFromSourceCode(QGLShader::Fragment, fragmentShader());

    char const *const *attr = attributeNames();
#ifndef QT_NO_DEBUG
    int maxVertexAttribs = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    for (int i = 0; attr[i]; ++i) {
        if (i >= maxVertexAttribs) {
            qFatal("List of attribute names is either too long or not null-terminated.\n"
                   "Maximum number of attributes on this hardware is %i.\n"
                   "Vertex shader:\n%s\n"
                   "Fragment shader:\n%s\n",
                   maxVertexAttribs, vertexShader(), fragmentShader());
        }
        if (*attr[i])
            m_program.bindAttributeLocation(attr[i], i);
    }
#else
    for (int i = 0; attr[i]; ++i) {
        if (*attr[i])
            m_program.bindAttributeLocation(attr[i], i);
    }
#endif

    if (!m_program.link()) {
        qWarning("QSGMaterialShader: Shader compilation failed:");
        qWarning() << m_program.log();
    }

    m_compiled = true;
    initialize();
}


float QSGMaterialShader::RenderState::opacity() const
{
    Q_ASSERT(m_data);
    return static_cast<const QSGRenderer *>(m_data)->renderOpacity();
}

QMatrix4x4 QSGMaterialShader::RenderState::combinedMatrix() const
{
    Q_ASSERT(m_data);
    return static_cast<const QSGRenderer *>(m_data)->combinedMatrix();
}

QMatrix4x4 QSGMaterialShader::RenderState::modelViewMatrix() const
{
    Q_ASSERT(m_data);
    return const_cast<QSGRenderer *>(static_cast<const QSGRenderer *>(m_data))->modelViewMatrix().top();
}

QRect QSGMaterialShader::RenderState::viewportRect() const
{
    Q_ASSERT(m_data);
    return static_cast<const QSGRenderer *>(m_data)->viewportRect();
}

QRect QSGMaterialShader::RenderState::deviceRect() const
{
    Q_ASSERT(m_data);
    return static_cast<const QSGRenderer *>(m_data)->deviceRect();
}

const QGLContext *QSGMaterialShader::RenderState::context() const
{
    return static_cast<const QSGRenderer *>(m_data)->glContext();
}


#ifndef QT_NO_DEBUG
static int qt_material_count = 0;

static void qt_print_material_count()
{
    qDebug("Number of leaked materials: %i", qt_material_count);
    qt_material_count = -1;
}
#endif

QSGMaterial::QSGMaterial()
    : m_flags(0)
{
#ifndef QT_NO_DEBUG
    ++qt_material_count;
    static bool atexit_registered = false;
    if (!atexit_registered) {
        atexit(qt_print_material_count);
        atexit_registered = true;
    }
#endif
}

QSGMaterial::~QSGMaterial()
{
#ifndef QT_NO_DEBUG
    --qt_material_count;
    if (qt_material_count < 0)
        qDebug("Material destroyed after qt_print_material_count() was called.");
#endif
}

void QSGMaterial::setFlag(Flags flags, bool set)
{
    if (set)
        m_flags |= flags;
    else
        m_flags &= ~flags;
}

int QSGMaterial::compare(const QSGMaterial *other) const
{
    Q_ASSERT(other && type() == other->type());
    return qint64(this) - qint64(other);
}

QT_END_NAMESPACE
