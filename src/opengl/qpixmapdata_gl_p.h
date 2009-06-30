/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPIXMAPDATA_GL_P_H
#define QPIXMAPDATA_GL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgl.h"

#include "private/qpixmapdata_p.h"

QT_BEGIN_NAMESPACE

class QPaintEngine;
class QGLFramebufferObject;

class QGLPixmapData : public QPixmapData
{
public:
    QGLPixmapData(PixelType type);
    ~QGLPixmapData();

    bool isValid() const;

    void resize(int width, int height);
    void fromImage(const QImage &image,
                   Qt::ImageConversionFlags flags);
    void copy(const QPixmapData *data, const QRect &rect);

    bool scroll(int dx, int dy, const QRect &rect);

    void fill(const QColor &color);
    bool hasAlphaChannel() const;
    QImage toImage() const;
    QPaintEngine* paintEngine() const;

    GLuint bind(bool copyBack = true) const;
    GLuint textureId() const;

    bool isValidContext(const QGLContext *ctx) const;

    void ensureCreated() const;

    bool isUninitialized() const { return m_dirty && m_source.isNull(); }

    bool needsFill() const { return m_hasFillColor; }
    QColor fillColor() const { return m_fillColor; }

    QSize size() const { return QSize(w, h); }

    QGLFramebufferObject *fbo() const;

    void makeCurrent();
    void doneCurrent();
    void swapBuffers();

protected:
    int metric(QPaintDevice::PaintDeviceMetric metric) const;

private:
    QGLPixmapData(const QGLPixmapData &other);
    QGLPixmapData &operator=(const QGLPixmapData &other);

    void copyBackFromRenderFbo(bool keepCurrentFboBound) const;

    static bool useFramebufferObjects();

    QImage fillImage(const QColor &color) const;

    mutable QGLFramebufferObject *m_renderFbo;
    mutable GLuint m_textureId;
    mutable QPaintEngine *m_engine;
    mutable QGLContext *m_ctx;
    mutable QImage m_source;

    // the texture is not in sync with the source image
    mutable bool m_dirty;

    // fill has been called and no painting has been done, so the pixmap is
    // represented by a single fill color
    mutable QColor m_fillColor;
    mutable bool m_hasFillColor;

    mutable bool m_hasAlpha;
};

QT_END_NAMESPACE

#endif // QPIXMAPDATA_GL_P_H


