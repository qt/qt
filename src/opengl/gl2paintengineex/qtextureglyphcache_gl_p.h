/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QTEXTUREGLYPHCACHE_GL_P_H
#define QTEXTUREGLYPHCACHE_GL_P_H

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

#include <private/qtextureglyphcache_p.h>
#include <private/qgl_p.h>
#include <qglshaderprogram.h>


QT_BEGIN_NAMESPACE

class QGL2PaintEngineExPrivate;

class Q_OPENGL_EXPORT QGLTextureGlyphCache : public QObject, public QImageTextureGlyphCache
{
    Q_OBJECT
public:
    QGLTextureGlyphCache(QGLContext *context, QFontEngineGlyphCache::Type type, const QTransform &matrix);
    ~QGLTextureGlyphCache();

    virtual void createTextureData(int width, int height);
    virtual void resizeTextureData(int width, int height);
    virtual void fillTexture(const Coord &c, glyph_t glyph);
    virtual int glyphPadding() const;
    virtual int maxTextureWidth() const;
    virtual int maxTextureHeight() const;

    inline GLuint texture() const { return m_texture; }

    inline int width() const { return m_width; }
    inline int height() const { return m_height; }

    inline void setPaintEnginePrivate(QGL2PaintEngineExPrivate *p) { pex = p; }

    enum FilterMode {
        Nearest,
        Linear
    };
    FilterMode filterMode() const { return m_filterMode; }
    void setFilterMode(FilterMode m) { m_filterMode = m; }

public Q_SLOTS:
    void contextDestroyed(const QGLContext *context) {
        if (context == ctx) {
            const QGLContext *nextCtx = qt_gl_transfer_context(ctx);
            if (!nextCtx) {
                // the context may not be current, so we cannot directly
                // destroy the fbo and texture here, but since the context
                // is about to be destroyed, the GL server will do the
                // clean up for us anyway
                m_fbo = 0;
                m_texture = 0;
                ctx = 0;
            } else {
                // since the context holding the texture is shared, and
                // about to be destroyed, we have to transfer ownership
                // of the texture to one of the share contexts
                ctx = const_cast<QGLContext *>(nextCtx);
            }
        }
    }

private:
    QGLContext *ctx;

    QGL2PaintEngineExPrivate *pex;

    GLuint m_texture;
    GLuint m_fbo;

    int m_width;
    int m_height;

    QGLShaderProgram *m_program;

    FilterMode m_filterMode;
};

QT_END_NAMESPACE

#endif

