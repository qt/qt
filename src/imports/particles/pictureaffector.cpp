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

#include "pictureaffector.h"
#include "coloredparticle.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

PictureAffector::PictureAffector(QSGItem *parent) :
    ParticleAffector(parent)
{
    m_needsReset = true;
}

void PictureAffector::reset(int systemIdx)
{
    ParticleAffector::reset(systemIdx);
}

bool PictureAffector::affectParticle(ParticleData *d, qreal dt)
{
    Q_UNUSED(dt);
    if(!width() || !height()){
        qWarning() << "PictureAffector needs a size";
        return false;
    }

    if(m_loadedImage.isNull())
        return false;

    if(m_loadedImage.size()!=QSize(width(), height()))
        m_loadedImage = m_loadedImage.scaled(width(), height());//TODO: Aspect Ratio Control?

    bool affected = false;
    QPoint pos = QPoint(d->curX() - m_offset.x(), d->curY() - m_offset.y());
    if(!QRect(0,0,width(),height()).contains(pos)){
        //XXX: Just a debugging helper, as I don't think it can get here.
        qWarning() << "An unexpected situation has occurred. But don't worry, everything will be fine.";
        return false;
    }
    Color4ub c;
    QRgb col = m_loadedImage.pixel(pos);
    c.a = qAlpha(col);
    c.b = qBlue(col);
    c.g = qGreen(col);
    c.r = qRed(col);
    foreach(ParticleType *p, m_system->m_groupData[d->group]->types){
       if(qobject_cast<ColoredParticle*>(p)){
           ColoredParticle* cp = qobject_cast<ColoredParticle*>(p);
           cp->reloadColor(c, d);
           affected = true;
       }
    }

    return affected;//Doesn't affect particle data, but necessary for onceOff
}

QT_END_NAMESPACE
