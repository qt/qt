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

#ifndef _GLTEXTURE_H_
#define _GLTEXTURE_H_

#include <QtDeclarative/qfxglobal.h>

#include <QtCore/QRect>
#include <QtCore/QPoint>
#include "glheaders.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QString;
class QImage;
class GLTexturePrivate;
class Q_DECLARATIVE_EXPORT GLTexture
{
public:
    GLTexture();
    GLTexture(const QString &file);
    GLTexture(const QImage &img);
    virtual ~GLTexture();

    bool isNull() const;
    void clear();

    enum ImageMode { NonPowerOfTwo, PowerOfTwo };
    void setImage(const QImage &, ImageMode = NonPowerOfTwo);
    void copyImage(const QImage &, const QPoint & = QPoint(0, 0), const QRect & = QRect());

    int width() const;
    int height() const;
    qreal glWidth() const;
    qreal glHeight() const;
    QSize glSize() const;
    QSize size() const;
    void setSize(const QSize &);

    enum WrapMode {
        Repeat = GL_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
#if defined(QFX_RENDER_OPENGL2)
        MirroredRepeat = GL_MIRRORED_REPEAT,
#else
        MirroredRepeat = Repeat
#endif
    };

    WrapMode horizontalWrap() const;
    WrapMode verticalWrap() const;
    void setHorizontalWrap(WrapMode);
    void setVerticalWrap(WrapMode);

    enum FilterMode {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        MipmapLinear = GL_LINEAR_MIPMAP_LINEAR
    };

    FilterMode minFilter() const;
    FilterMode magFilter() const;
    void setMinFilter(FilterMode);
    void setMagFilter(FilterMode);

    GLuint texture() const;
private:
    Q_DISABLE_COPY(GLTexture)
    GLTexturePrivate *d;
};


QT_END_NAMESPACE

QT_END_HEADER
#endif // _GLTEXTURE_H_
