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

#include <qgl.h>

#include "qsgtexture.h"
#include "qobject.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_EXPORT QSGTextureProvider : public QObject
{
    Q_OBJECT
public:
    enum WrapMode {
        Repeat,
        ClampToEdge
    };

    enum Filtering {
        None,
        Nearest,
        Linear
    };

    QSGTextureProvider(QObject *parent = 0);
    virtual void updateTexture() { }
    virtual QSGTextureRef texture() = 0;

    virtual bool isStaticTexture() const;

    void bind(QSGTexture *oldTexture = 0);

    virtual WrapMode horizontalWrapMode() const = 0;
    virtual WrapMode verticalWrapMode() const = 0;
    virtual Filtering filtering() const = 0;
    virtual Filtering mipmapFiltering() const = 0;

    GLint glTextureWrapS() const;
    GLint glTextureWrapT() const;
    GLint glMinFilter() const;
    GLint glMagFilter() const;

Q_SIGNALS:
    void textureChanged();
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
