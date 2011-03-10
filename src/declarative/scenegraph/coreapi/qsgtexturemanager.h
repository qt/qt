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

#ifndef QSGTEXTUREMANAGER_H
#define QSGTEXTUREMANAGER_H

#include "qsgcontext.h"

#include <QObject>
#include <QImage>
#include <QQueue>

class QSGTextureManagerPrivate;

class Q_DECLARATIVE_EXPORT QSGTexture : public QObject
{
    Q_OBJECT

public:
    enum Status {
        Null,
        InProgress,
        Ready
    };

    QSGTexture();
    ~QSGTexture();

    void setTextureId(int id) { m_texture_id = id; }
    int textureId() const { return m_texture_id; }

    void setTextureSize(const QSize &size) { m_texture_size = size; }
    QSize textureSize() const { return m_texture_size; }

    void setAlphaChannel(bool hasAlpha) { m_has_alpha = hasAlpha; }
    bool hasAlphaChannel() const { return m_has_alpha; }

    void setOwnsTexture(bool does) { m_owns_texture = does; }
    bool ownsTexture() const { return m_owns_texture; }

    void setHasMipmaps(bool has) { m_has_mipmaps = has; }
    bool hasMipmaps() const { return m_has_mipmaps; }

    void setStatus(Status s);
    Status status() const { return m_status; }

    QRectF subRect() const { return m_sub_rect; }
    void setSubRect(const QRectF &subrect);

public:
    Status m_status;
    int m_texture_id;
    mutable int m_ref_count;

    QSize m_texture_size;
    QRectF m_sub_rect;

    uint m_has_alpha : 1;
    uint m_owns_texture : 1;
    uint m_has_mipmaps : 1;

    friend class QSGTextureRef;
};


class Q_DECLARATIVE_EXPORT QSGTextureRef
{
public:
    QSGTextureRef()
        : m_texture(0)
    {
    }

    QSGTextureRef(const QSGTexture *texture, const QRectF &subrect = QRectF(0, 0, 1, 1))
        : m_texture(texture)
    {
        if (texture)
            ++texture->m_ref_count;
    }

    QSGTextureRef(const QSGTextureRef &other)
    {
        m_texture = other.m_texture;
        if (m_texture)
            ++m_texture->m_ref_count;
    }

    ~QSGTextureRef()
    {
        deref();
    }

    const QSGTexture *texture() const { return m_texture; }
    const QSGTexture *operator->() const { return m_texture; }

    QSGTextureRef &operator=(const QSGTextureRef &other)
    {
        if (other.m_texture)
            ++other.m_texture->m_ref_count;
        deref();
        m_texture = other.m_texture;
        m_sub_rect = other.m_sub_rect;

        return *this;
    }

    bool isNull() const { return m_texture == 0; }

    bool isReady() const { return m_texture != 0 && m_texture->status() == QSGTexture::Ready; }

private:
    void deref() {
        if (m_texture && !--m_texture->m_ref_count) {
            delete m_texture;
        }
    }

    const QSGTexture *m_texture;
    QRectF m_sub_rect;
};

class QSGTextureUploadRequestPrivate;
class Q_DECLARATIVE_EXPORT QSGTextureUploadRequest : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGTextureUploadRequest);
public:
    QSGTextureUploadRequest();

    void setImage(const QImage &image);
    QImage image() const;

    QSGTextureRef texture() const;
    void setTexture(const QSGTextureRef &ref);

public slots:
    virtual void done();

signals:
    void requestCompleted(QSGTextureUploadRequest *);
};

class QSGTextureManagerPrivate;
class Q_DECLARATIVE_EXPORT QSGTextureManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGTextureManager);

public:
    QSGTextureManager();

    virtual QSGTextureRef upload(const QImage &image);
    virtual void requestUpload(QSGTextureUploadRequest *request);

    static void swizzleBGRAToRGBA(QImage *image);

    int maxTextureSize() const;

private slots:
    void textureDestroyed(QObject *texture);

protected:
    QSGTextureManager(QSGTextureManagerPrivate &);
};

#endif // QSGTEXTUREMANAGER_H
