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

#ifndef SHADEREFFECTSOURCE_H
#define SHADEREFFECTSOURCE_H

#include "qsgitem.h"
#include "qsgtextureprovider.h"
#include "textureitem.h"

#include "qpointer.h"
#include "qsize.h"
#include "qrect.h"

//#define QML_SUBTREE_DEBUG

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Node;
class UpdatePaintNodeData;

class ShaderEffectTextureProvider : public QSGTextureProvider
{
    Q_OBJECT
public:
    ShaderEffectTextureProvider(QObject *parent = 0);
    ~ShaderEffectTextureProvider();
    virtual void updateTexture();
    virtual QSGTextureRef texture();

    // The item's "paint node", not effect node.
    Node *item() const { return m_item; }
    void setItem(Node *item);

    QRectF rect() const { return m_rect; }
    void setRect(const QRectF &rect);

    QSize size() const { return m_size; }
    void setSize(const QSize &size);

    GLenum format() const { return m_format; }
    void setFormat(GLenum format);

    bool live() const { return bool(m_live); }
    void setLive(bool live);

    void grab();

private Q_SLOTS:
    void markDirtyTexture();

private:
    Node *m_item;
    QRectF m_rect;
    QSize m_size;
    GLenum m_format;

    Renderer *m_renderer;
    QGLFramebufferObject *m_fbo;
    QSGTextureRef m_texture;

#ifdef QML_SUBTREE_DEBUG
    RectangleNodeInterface *m_debugOverlay;
#endif

    uint m_live : 1;
    uint m_dirtyTexture : 1;
};

class ShaderEffectSource : public TextureItem
{
    Q_OBJECT
    Q_PROPERTY(QSGItem *sourceItem READ sourceItem WRITE setSourceItem NOTIFY sourceItemChanged)
    Q_PROPERTY(QRectF sourceRect READ sourceRect WRITE setSourceRect NOTIFY sourceRectChanged)
    Q_PROPERTY(QSize textureSize READ textureSize WRITE setTextureSize NOTIFY textureSizeChanged)
    Q_PROPERTY(Format format READ format WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(bool live READ live WRITE setLive NOTIFY liveChanged)
    Q_PROPERTY(bool hideSource READ hideSource WRITE setHideSource NOTIFY hideSourceChanged)
    Q_ENUMS(Format)
public:
    enum Format {
        Alpha = GL_ALPHA,
        RGB = GL_RGB,
        RGBA = GL_RGBA
    };

    ShaderEffectSource(QSGItem *parent = 0);
    ~ShaderEffectSource();

    QSGItem *sourceItem() const;
    void setSourceItem(QSGItem *item);

    QRectF sourceRect() const;
    void setSourceRect(const QRectF &rect);

    QSize textureSize() const;
    void setTextureSize(const QSize &size);

    Format format() const;
    void setFormat(Format format);

    bool live() const;
    void setLive(bool live);

    bool hideSource() const;
    void setHideSource(bool hide);

    Q_INVOKABLE void grab();

Q_SIGNALS:
    void sourceItemChanged();
    void sourceRectChanged();
    void textureSizeChanged();
    void formatChanged();
    void liveChanged();
    void hideSourceChanged();

protected:
    virtual Node *updatePaintNode(Node *, UpdatePaintNodeData *);

private:
    QPointer<QSGItem> m_sourceItem;
    QRectF m_sourceRect;
    QSize m_textureSize;
    Format m_format;
    uint m_live : 1;
    uint m_hideSource : 1;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // SHADEREFFECTSOURCE_H
