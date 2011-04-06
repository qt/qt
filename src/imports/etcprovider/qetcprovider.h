/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
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

#ifndef QETCPROVIDER_H
#define QETCPROVIDER_H

#include <qgl.h>
#include <QDeclarativeImageProvider>
#include <QSGTexture>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFileInfo>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

// #define ETC_DEBUG

class EtcTexture : public QSGTexture
{
    Q_OBJECT
public:
    EtcTexture();
    ~EtcTexture();

    void bind();
    QSize textureSize() const;

    int textureId() const { return m_texture_id; }

    void setImage(const QImage &image) { Q_UNUSED(image); }

    bool hasAlphaChannel() const { return false; }
    bool hasMipmaps() const { return false; }

    QByteArray m_data;
    QSize m_size;
    QSize m_paddedSize;
    GLuint m_texture_id;
};

class QEtcProvider : public QDeclarativeImageProvider
{
public:
    QEtcProvider()
        : QDeclarativeImageProvider(QDeclarativeImageProvider::Texture)
    {
#ifdef ETC_DEBUG
        qDebug () << "Creating QEtcProvider.";
#endif
    }
    QSGTexture *requestTexture(const QString &id, QSize *size, const QSize &requestedSize);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QETCPROVIDER_H
