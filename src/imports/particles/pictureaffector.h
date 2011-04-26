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

#ifndef PICTUREAFFECTOR_H
#define PICTUREAFFECTOR_H
#include "particleaffector.h"
#include <QDebug>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class PictureAffector : public ParticleAffector
{
    Q_OBJECT
    //Usually want to use "particles" to target just colored stuff, and save performance
    //Use onceOff (inherited) to determine if this is an emitter modification or a more constant enforcer
    //TODO: Onceoff isn't actually working right now...
    Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY imageChanged)
    //TODO: Bool smooth, where it interpolates
public:
    explicit PictureAffector(QSGItem *parent = 0);

    QUrl image() const
    {
        return m_image;
    }

protected:
    virtual void reset(int systemIdx);
    virtual bool affectParticle(ParticleData *d, qreal dt);
signals:

    void imageChanged(QUrl arg);

public slots:
    void setImage(QUrl arg)
    {
        if (m_image != arg) {
            m_image = arg;
            m_loadedImage = QImage(m_image.toLocalFile());
            if(m_loadedImage.isNull())
                qWarning() << "PictureAffector could not load picture " << m_image.toLocalFile();
            emit imageChanged(arg);
        }
    }

private:
    QUrl m_image;
    QImage m_loadedImage;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // PICTUREAFFECTOR_H
