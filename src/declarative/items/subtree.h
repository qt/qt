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

#ifndef SUBTREE_H
#define SUBTREE_H

#include "qsgitem.h"
#include "qsgtexturemanager.h"
#include "texturematerial.h"
#include "node.h"
#include "qobject.h"
#include "qpointer.h"

//#define QML_SUBTREE_DEBUG

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGTextureProvider;
class Renderer;
class QGLFramebufferObject;

class TextureNode : public QObject, public GeometryNode
{
    Q_OBJECT
public:
    TextureNode();
    void setTargetRect(const QRectF &rect);
    void setSourceRect(const QRectF &rect);
    void setOpacity(qreal opacity);
    void setTexture(QSGTextureProvider *texture);

    virtual void preprocess();

private slots:
    void markDirtyTexture();

private:
    void updateGeometry();
    void updateTexture();

    TextureMaterial m_material;
    TextureMaterialWithOpacity m_materialO;

    QSGTextureProvider *m_texture;
    QRectF m_targetRect;
    QRectF m_sourceRect;
    qreal m_opacity;

    uint m_dirtyTexture : 1;
    uint m_dirtyGeometry : 1;
};


class SubTreeTextureProvider : public QSGTextureProvider
{
    Q_OBJECT
public:
    SubTreeTextureProvider(QObject *parent = 0);
    ~SubTreeTextureProvider();
    virtual void updateTexture();
    virtual QSGTextureRef texture();

    // The item's "paint node".
    Node *item() const { return m_item; }
    void setItem(Node *item);

    QRectF rect() const { return m_rect; }
    void setRect(const QRectF &rect);

    bool live() const { return bool(m_live); }
    void setLive(bool live);

    void grab();

private Q_SLOTS:
    void markDirtyTexture();

private:
    Node *m_item;
    QRectF m_rect;

    Renderer *m_renderer;
    QGLFramebufferObject *m_fbo;
    QSGTextureRef m_texture;

#ifdef QML_SUBTREE_DEBUG
    RectangleNodeInterface *m_debugOverlay;
#endif

    uint m_live : 1;
    uint m_dirtyTexture : 1;
};


// TODO: Find good name.
class TextureProviderInterface
{
public:
    virtual QSGTextureProvider *textureProvider() const = 0;
};
Q_DECLARE_INTERFACE(TextureProviderInterface, "TextureProviderInterface")


class TextureItem : public QSGItem, public TextureProviderInterface
{
    Q_OBJECT
    Q_INTERFACES(TextureProviderInterface)
    // TODO: property clampToEdge
    // TODO: property mipmapFiltering
public:
    TextureItem(QSGItem *parent = 0);

    virtual QSGTextureProvider *textureProvider() const;
    void setTextureProvider(QSGTextureProvider *provider);
    
protected:
    virtual Node *updatePaintNode(Node *, UpdatePaintNodeData *);

    QSGTextureProvider *m_textureProvider;
};


class SubTree : public TextureItem
{
    Q_OBJECT
    Q_PROPERTY(QSGItem *item READ item WRITE setItem NOTIFY itemChanged)
    Q_PROPERTY(QSizeF margins READ margins WRITE setMargins NOTIFY marginsChanged)
    Q_PROPERTY(bool live READ live WRITE setLive NOTIFY liveChanged)
public:
    SubTree(QSGItem *parent = 0);
    ~SubTree();

    QSGItem *item() const;
    void setItem(QSGItem *item);

    QSizeF margins() const;
    void setMargins(const QSizeF &margins);

    bool live() const;
    void setLive(bool live);

    Q_INVOKABLE void grab();

Q_SIGNALS:
    void itemChanged();
    void marginsChanged();
    void liveChanged();

protected:
    virtual Node *updatePaintNode(Node *, UpdatePaintNodeData *);

private:
    QPointer<QSGItem> m_item;
    QSizeF m_margins;
    uint m_live : 1;
};

QT_END_NAMESPACE
//QML_DECLARE_TYPE(SubTree)
QT_END_HEADER

#endif
