/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLFRAMEBUFFEROBJECT_P_H
#define QGLFRAMEBUFFEROBJECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

QT_BEGIN_INCLUDE_NAMESPACE

#include <qglframebufferobject.h>
#include <private/qglpaintdevice_p.h>
#include <private/qgl_p.h>

QT_END_INCLUDE_NAMESPACE

#ifndef QT_OPENGL_ES
#define DEFAULT_FORMAT GL_RGBA8
#else
#define DEFAULT_FORMAT GL_RGBA
#endif

class QGLFramebufferObjectFormatPrivate
{
public:
    QGLFramebufferObjectFormatPrivate()
        : ref(1),
          samples(0),
          attachment(QGLFramebufferObject::NoAttachment),
          target(GL_TEXTURE_2D),
          internal_format(DEFAULT_FORMAT),
          mipmap(false)
    {
    }
    QGLFramebufferObjectFormatPrivate
            (const QGLFramebufferObjectFormatPrivate *other)
        : ref(1),
          samples(other->samples),
          attachment(other->attachment),
          target(other->target),
          internal_format(other->internal_format),
          mipmap(other->mipmap)
    {
    }
    bool equals(const QGLFramebufferObjectFormatPrivate *other)
    {
        return samples == other->samples &&
               attachment == other->attachment &&
               target == other->target &&
               internal_format == other->internal_format &&
               mipmap == other->mipmap;
    }

    QAtomicInt ref;
    int samples;
    QGLFramebufferObject::Attachment attachment;
    GLenum target;
    GLenum internal_format;
    uint mipmap : 1;
};

class QGLFBOGLPaintDevice : public QGLPaintDevice
{
public:
    virtual QPaintEngine* paintEngine() const {return fbo->paintEngine();}
    virtual QSize size() const {return fbo->size();}
    virtual QGLContext* context() const;
    virtual QGLFormat format() const {return fboFormat;}
    virtual bool alphaRequested() const { return reqAlpha; }

    void setFBO(QGLFramebufferObject* f,
                QGLFramebufferObject::Attachment attachment);

private:
    QGLFramebufferObject* fbo;
    QGLFormat fboFormat;
    bool wasBound;
    bool reqAlpha;
};

class QGLFramebufferObjectPrivate
{
public:
    QGLFramebufferObjectPrivate() : fbo_guard(0), texture(0), depth_buffer(0), stencil_buffer(0)
                                  , color_buffer(0), valid(false), engine(0) {}
    ~QGLFramebufferObjectPrivate() {}

    void init(QGLFramebufferObject *q, const QSize& sz,
              QGLFramebufferObject::Attachment attachment,
              GLenum internal_format, GLenum texture_target,
              GLint samples = 0, bool mipmap = false);
    bool checkFramebufferStatus() const;
    QGLSharedResourceGuard fbo_guard;
    GLuint texture;
    GLuint depth_buffer;
    GLuint stencil_buffer;
    GLuint color_buffer;
    GLenum target;
    QSize size;
    QGLFramebufferObjectFormat format;
    uint valid : 1;
    QGLFramebufferObject::Attachment fbo_attachment;
    mutable QPaintEngine *engine;
    QGLFBOGLPaintDevice glDevice;

    inline GLuint fbo() const { return fbo_guard.id(); }
};


QT_END_NAMESPACE

#endif // QGLFRAMEBUFFEROBJECT_P_H
