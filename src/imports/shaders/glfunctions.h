/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
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

#ifndef GLFUNCTIONS_H
#define GLFUNCTIONS_H

#ifndef QT_OPENGL_ES

#ifndef Q_WS_MAC
# ifndef APIENTRYP
#   ifdef APIENTRY
#     define APIENTRYP APIENTRY *
#   else
#     define APIENTRY
#     define APIENTRYP *
#   endif
# endif
#else
# define APIENTRY
# define APIENTRYP *
#endif

#define GL_TEXTURE0 0x84C0
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_BGRA 0x80E1

typedef void (APIENTRYP type_glActiveTexture)(GLenum texture);
typedef void (APIENTRYP type_glGenerateMipmap)(GLenum target);
typedef void (APIENTRYP type_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);

#define glActiveTexture ((type_glActiveTexture)QGLContext::currentContext()->getProcAddress(QLatin1String("glActiveTexture")))
#define glGenerateMipmap ((type_glGenerateMipmap)QGLContext::currentContext()->getProcAddress(QLatin1String("glGenerateMipmap")))
#define glVertexAttribPointer ((type_glVertexAttribPointer)QGLContext::currentContext()->getProcAddress(QLatin1String("glVertexAttribPointer")))

#endif

#endif // GLFUNCTIONS_H
