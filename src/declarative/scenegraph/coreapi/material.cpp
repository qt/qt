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

#include "material.h"

AbstractShaderEffectProgram::AbstractShaderEffectProgram()
    : m_initialized(false)
{
}

void AbstractShaderEffectProgram::activate()
{
    if (!m_initialized)
        initialize();

    m_program.bind();
    const Attributes attr = attributes();
    for (int i = 0; attr.names[i]; ++i)
        m_program.enableAttributeArray(attr.ids[i]);
}

void AbstractShaderEffectProgram::deactivate()
{
    const Attributes attr = attributes();
    for (int i = 0; attr.names[i]; ++i)
        m_program.disableAttributeArray(attr.ids[i]);
}

const QSG::VertexAttribute *AbstractShaderEffectProgram::requiredFields() const
{
    return attributes().ids;
}

void AbstractShaderEffectProgram::initialize()
{
    Q_ASSERT(!m_initialized);

    m_program.addShaderFromSourceCode(QGLShader::Vertex, vertexShader());
    m_program.addShaderFromSourceCode(QGLShader::Fragment, fragmentShader());

    const Attributes attr = attributes();
    for (int i = 0; attr.names[i]; ++i)
        m_program.bindAttributeLocation(attr.names[i], attr.ids[i]);

    m_program.link();

    m_initialized = true;
}

#ifndef QT_NO_DEBUG
static int qt_material_count = 0;

static void qt_print_material_count()
{
    qDebug("Number of leaked materials: %i", qt_material_count);
    qt_material_count = -1;
}
#endif

AbstractEffect::AbstractEffect() : m_flags(0)
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

AbstractEffect::~AbstractEffect()
{
#ifndef QT_NO_DEBUG
    --qt_material_count;
    if (qt_material_count < 0)
        qDebug("Material destroyed after qt_print_material_count() was called.");
#endif
}
