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

#include "adaptationlayer.h"
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>

class QGLFramebufferObject;

class ShaderEffectSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSGItem *sourceItem READ sourceItem WRITE setSourceItem NOTIFY sourceItemChanged)
    Q_PROPERTY(QUrl sourceImage READ sourceImage WRITE setSourceImage NOTIFY sourceImageChanged)
    Q_PROPERTY(FilterMode mipmap READ mipmap WRITE setMipmap NOTIFY mipmapChanged)
    Q_PROPERTY(FilterMode filtering READ filtering WRITE setFiltering NOTIFY filteringChanged)
    Q_PROPERTY(WrapMode horizontalWrap READ horizontalWrap WRITE setHorizontalWrap NOTIFY horizontalWrapChanged)
    Q_PROPERTY(WrapMode verticalWrap READ verticalWrap WRITE setVerticalWrap NOTIFY verticalWrapChanged)
    Q_PROPERTY(QSize margins READ margins WRITE setMargins NOTIFY marginsChanged)
    Q_PROPERTY(QSize textureSize READ textureSize WRITE setTextureSize NOTIFY textureSizeChanged)
    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int height READ height NOTIFY heightChanged)
    Q_PROPERTY(bool live READ isLive WRITE setLive NOTIFY liveChanged)
    Q_PROPERTY(bool hideOriginal READ hideOriginal WRITE setHideOriginal NOTIFY hideOriginalChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_ENUMS(FilterMode)
    Q_ENUMS(WrapMode)

public:
    enum FilterMode
    {
        None,
        Nearest,
        Linear
    };

    enum WrapMode
    {
        Repeat,
        ClampToEdge
    };

    ShaderEffectSource(QObject *parent = 0);
    virtual ~ShaderEffectSource();

    QSGContext *sceneGraphContext() const { return m_context; }
    void setSceneGraphContext(QSGContext *context) { m_context = context; }

    QSGItem *sourceItem() const { return m_sourceItem.data(); }
    void setSourceItem(QSGItem *item);

    QUrl sourceImage() const { return m_sourceImage; }
    void setSourceImage(const QUrl &url);

    FilterMode mipmap() const { return m_mipmap; }
    void setMipmap(FilterMode mode);

    FilterMode filtering() const { return m_filtering; }
    void setFiltering(FilterMode mode);

    WrapMode horizontalWrap() const { return m_horizontalWrap; }
    void setHorizontalWrap(WrapMode mode);

    WrapMode verticalWrap() const { return m_verticalWrap; }
    void setVerticalWrap(WrapMode mode);

    QSize margins() const { return m_margins; }
    void setMargins(const QSize &size);

    QSize textureSize() const { return m_textureSize; }
    void setTextureSize(const QSize &size);

    int width() const { return m_size.width(); }
    int height() const { return m_size.height(); }

    bool isLive() const { return m_live; }
    void setLive(bool s);

    bool hideOriginal() const { return m_hideOriginal; }
    void setHideOriginal(bool hide);

    bool isActive() const { return m_refs; }

    void bind() const;

    void refFromEffectItem();
    void derefFromEffectItem();
    void update();

    Q_INVOKABLE void grab();

Q_SIGNALS:
    void sourceItemChanged();
    void sourceImageChanged();
    void mipmapChanged();
    void filteringChanged();
    void horizontalWrapChanged();
    void verticalWrapChanged();
    void marginsChanged();
    void textureSizeChanged();
    void widthChanged();
    void heightChanged();
    void liveChanged();
    void hideOriginalChanged();
    void activeChanged();

    void repaintRequired();

private Q_SLOTS:
    void markSceneGraphDirty();
    void markSourceSizeDirty();

private:
    void updateSizeAndTexture();

    QPointer<QSGItem> m_sourceItem;
    QUrl m_sourceImage;
    FilterMode m_mipmap;
    FilterMode m_filtering;
    WrapMode m_horizontalWrap;
    WrapMode m_verticalWrap;
    QSize m_margins;
    QSize m_textureSize;
    QSize m_size;

    QImage m_image;
    QSGTextureRef m_texture;
    QGLFramebufferObject *m_fbo;
    QGLFramebufferObject *m_multisampledFbo;
    Renderer *m_renderer;
    QSGContext *m_context;
    int m_refs;
    uint m_dirtyTexture : 1; // Causes update no matter what.
    uint m_dirtySceneGraph : 1; // Causes update if not static.
    uint m_multisamplingSupported : 1;
    uint m_checkedForMultisamplingSupport : 1;
    uint m_live : 1;
    uint m_hideOriginal : 1;
};

#endif // SHADEREFFECTSOURCE_H
