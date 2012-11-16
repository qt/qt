/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

#ifdef QT_OPENGL_ES_1_CL

//! [0]
#define FLOAT2X(f)      ((int) ( (f) * (65536)))
#define X2FLOAT(x)      ((float)(x) / 65536.0f)

#define f2vt(f)     FLOAT2X(f)
#define vt2f(x)     X2FLOAT(x)

#define q_vertexType GLfixed
#define q_vertexTypeEnum GL_FIXED

#define q_glFog             glFogx
#define q_glFogv            glFogxv
//! [0]

#define q_glLight           glLightx
#define q_glLightv          glLightxv
#define q_glLightModel      glLightModelx
#define q_glLightModelv     glLightModelxv

#define q_glAlphaFunc       glAlphaFuncx

#define q_glMaterial        glMaterialx
#define q_glMaterialv       glMaterialxv
#define q_glColor4          glColor4x

#define q_glTexParameter    glTexParameterx
#define q_glTexEnv          glTexEnvx

#define q_glOrtho           glOrthox
#define q_glFrustum         glFrustumx

#define q_glTranslate       glTranslatex
#define q_glScale           glScalex
#define q_glRotate          glRotatex
#define q_glLoadMatrix      glLoadMatrixx

#define q_glClearColor      glClearColorx

#define q_glMultMatrix      glMultMatrixx

#define q_glNormal3         glNormal3x

#define q_glPolygonOffset   glPolygonOffsetx
#define q_glPointSize       glPointSizex

//! [1]
#else

#define f2vt(f)     (f)
#define vt2f(x)     (x)

#define q_vertexType GLfloat
#define q_vertexTypeEnum GL_FLOAT

#define q_glFog             glFogf
#define q_glFogv            glFogfv
//! [1]

#define q_glLight           glLightf
#define q_glLightv          glLightfv
#define q_glLightModel      glLightModelf
#define q_glLightModelv     glLightModelfv

#define q_glAlphaFunc       glAlphaFuncf

#define q_glMaterial        glMaterialf
#define q_glMaterialv       glMaterialfv
#define q_glColor4          glColor4f

#define q_glTexParameter    glTexParameterf
#define q_glTexEnv          glTexEnvf

#define q_glOrtho           glOrthof
#define q_glFrustum         glFrustumf

#define q_glTranslate       glTranslatef
#define q_glScale           glScalef
#define q_glRotate          glRotatef
#define q_glLoadMatrix      glLoadMatrixf

#define q_glClearColor      glClearColor

#define q_glMultMatrix      glMultMatrixf

#define q_glNormal3         glNormal3f

#define q_glPolygonOffset   glPolygonOffsetf
#define q_glPointSize       glPointSizef

#endif
