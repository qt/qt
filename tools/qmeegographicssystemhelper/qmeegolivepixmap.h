/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
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
