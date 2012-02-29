/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEEGOFENCESYNC_H
#define QMEEGOFENCESYNC_H

#include <QtGui/QWidget>

class QMeeGoFenceSyncPrivate;

//! A synchronization helper for GL pipeline.
/*!
 Fence syncs provide a mechanism for synchronizing access to certain GL primitives
 and make it possible for the application developer to be sure that a certain point
 in the GL processing pipeline has been already executed before continuing operation.

 Currently fence syncs are only useful in conjunction with QMeeGoLivePixmaps.
 \code
 ...
 // In your paint/expose event:
 QImage *image = livePixmap->lock(&someGlobalFenceSync);
 // Modify the image...
 livePixmap->release(image);

 painter->drawPixmap(0, 0, *livePixmap);
 someGlobalFenceSync.setSyncPoint();
 ...
 \endcode

 Assuming the paint/expose events come repeatedly, the lock operation
 will block till the previous event completed painting the livePixmap.
*/

class Q_DECL_EXPORT QMeeGoFenceSync : public QObject
{
public:
    //! Constructs a new fence sync.
    /*!
     The fence sync is created without a sync point. You need to set the sync point manually.
    */
    QMeeGoFenceSync(QWidget *parent = 0);

    //! Destructor for the fence sync.
    virtual ~QMeeGoFenceSync();

    //! Sets the fence sync.
    /*!
      The fence sync synchronization point should be set after all drawing has been scheduled.
      Setting a synchronization point always overrides the previous point -- whetver is was
      used (waited upon) or not.
    */
    void setSyncPoint();

private:
    Q_DISABLE_COPY(QMeeGoFenceSync)
    Q_DECLARE_PRIVATE(QMeeGoFenceSync)

protected:
    QScopedPointer<QMeeGoFenceSyncPrivate> d_ptr; //! Private bits.
    friend class QMeeGoLivePixmap;
};

#endif
