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

#ifndef QSGTEXTUREPROVIDER_H
#define QSGTEXTUREPROVIDER_H

#include "qsgtexturemanager.h"
#include "qobject.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_EXPORT QSGTextureProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool opaque READ opaque WRITE setOpaque NOTIFY opaqueChanged)
    Q_PROPERTY(bool mipmap READ mipmap WRITE setMipmap NOTIFY mipmapChanged)
    Q_PROPERTY(bool clampToEdge READ clampToEdge WRITE setClampToEdge NOTIFY clampToEdgeChanged)
    Q_PROPERTY(bool linearFiltering READ linearFiltering WRITE setLinearFiltering NOTIFY linearFilteringChanged)
public:
    QSGTextureProvider(QObject *parent = 0);
    virtual void updateTexture() { }
    virtual QSGTextureRef texture() = 0;

    bool opaque() const { return m_opaque; }
    void setOpaque(bool enabled);

    bool mipmap() const { return m_mipmap; }
    void setMipmap(bool enabled);

    bool clampToEdge() const { return m_clampToEdge; }
    void setClampToEdge(bool enabled);

    bool linearFiltering() const { return m_linearFiltering; }
    void setLinearFiltering(bool enabled);

Q_SIGNALS:
    void opaqueChanged();
    void textureChanged();
    void mipmapChanged();
    void clampToEdgeChanged();
    void linearFilteringChanged();

protected:
    uint m_opaque : 1;
    uint m_mipmap : 1;
    uint m_clampToEdge : 1;
    uint m_linearFiltering : 1;
};


// TODO: Find good name.
class QSGTextureProviderInterface
{
public:
    virtual QSGTextureProvider *textureProvider() const = 0;
};
Q_DECLARE_INTERFACE(QSGTextureProviderInterface, "QSGTextureProviderInterface")


QT_END_NAMESPACE

QT_END_HEADER

#endif
