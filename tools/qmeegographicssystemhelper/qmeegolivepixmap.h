/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QMEEGOLIVEPIXMAP_H
#define QMEEGOLIVEPIXMAP_H

#include <QPixmap>
#include "qmeegoliveimage.h"

class QMeeGoLivePixmapPrivate;
class QSharedMemory;
class QImage;

//! A pixmap representing streamed content.
/*!
*/

class QMeeGoLivePixmap : public QPixmap
{
public:
    //! Creates new pixmap from the given QMeeGoLiveImage.
    /*! 
     The created QMeeGoLivePixmap will be attached to the given QMeeGoLiveImage. 
     Updates to the QMeeGoLiveImage will be represented on this newly created
     QMeeGoLivePixmap.
    */
    static QMeeGoLivePixmap* fromLiveImage(QMeeGoLiveImage *liveImage);
    
    //! Creates a new QMeeGoLivePixmap from the specified handle.
    /*! 
     The handle can be used to share QMeeGoLivePixmap cross-process.
    */
    static QMeeGoLivePixmap* fromHandle(Qt::HANDLE handle);
    
    //! Returns the handle for this QMeeGoLivePixmap.
    /*! 
     The handle can be used to share QMeeGoLivePixmap cross-process.
    */
    Qt::HANDLE handle();
    
    //! Destroys the QMeeGoLivePixmap.
    /*! 
     All QMeeGoLivePixmaps attached to a given QMeeGoLiveImage have to be destroyed 
     before the QMeeGoLiveImage itself is destroyed.
    */
    virtual ~QMeeGoLivePixmap();

private:
    QMeeGoLivePixmap(QPixmapData *p);
    Q_DISABLE_COPY(QMeeGoLivePixmap)
    Q_DECLARE_PRIVATE(QMeeGoLivePixmap)

protected:
    QScopedPointer<QMeeGoLivePixmapPrivate> d_ptr; //! Private bits.

    friend class QMeeGoLiveImage;
    friend class QMeeGoLiveImagePrivate;
};

#endif
