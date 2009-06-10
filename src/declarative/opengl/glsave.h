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

#ifndef _GLSAVE_H_
#define _GLSAVE_H_

#include <QtCore/qglobal.h>
#include <QtDeclarative/qfxglobal.h>
#include <QtCore/QRect>
#include "glheaders.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class GLSaveViewport
{
public:
    GLSaveViewport()
    {
        glGetIntegerv(GL_VIEWPORT, viewport);
    }

    ~GLSaveViewport()
    {
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    }

private:
    Q_DISABLE_COPY(GLSaveViewport)
    GLint viewport[4];
};

class GLSaveScissor
{
public:
    GLSaveScissor()
    {
        enabled = glIsEnabled(GL_SCISSOR_TEST);
        glGetIntegerv(GL_SCISSOR_BOX, box);
    }

    ~GLSaveScissor()
    {
        if (enabled)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
        glScissor(box[0], box[1], box[2], box[3]);
    }

    bool wasEnabled() const
    {
        return enabled == GL_TRUE;
    }

    QRect rect() const
    { 
        return QRect(box[0], box[1], box[2], box[3]);
    }

private:
    Q_DISABLE_COPY(GLSaveScissor)
    GLint box[4];
    GLboolean enabled;
};

QT_END_NAMESPACE

QT_END_HEADER
#endif // _GLSAVE_H_
