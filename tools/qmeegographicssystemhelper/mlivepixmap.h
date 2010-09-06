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

#ifndef MLIVEPIXMAP_H
#define MLIVEPIXMAP_H

#include <QPixmap>
#include "mliveimage.h"

class MLivePixmapPrivate;
class QSharedMemory;
class QImage;

//! A pixmap representing streamed content.
/*!
*/

class MLivePixmap : public QPixmap
{
public:
    //! Creates new pixmap from the given MLiveImage.
    /*! 
     The created MLivePixmap will be attached to the given MLiveImage. 
     Updates to the MLiveImage will be represented on this newly created
     MLivePixmap.
    */
    static MLivePixmap* fromLiveImage(MLiveImage *liveImage);
    
    //! Creates a new MLivePixmap from the specified handle.
    /*! 
     The handle can be used to share MLivePixmap cross-process.
    */
    static MLivePixmap* fromHandle(Qt::HANDLE handle);
    
    //! Returns the handle for this MLivePixmap.
    /*! 
     The handle can be used to share MLivePixmap cross-process.
    */
    Qt::HANDLE handle();
    
    //! Destroys the MLivePixmap.
    /*! 
     All MLivePixmaps attached to a given MLiveImage have to be destroyed 
     before the MLiveImage itself is destroyed.
    */
    virtual ~MLivePixmap();

private:
    MLivePixmap(QPixmapData *p);
    Q_DISABLE_COPY(MLivePixmap)
    Q_DECLARE_PRIVATE(MLivePixmap)

protected:
    QScopedPointer<MLivePixmapPrivate> d_ptr; //! Private bits.

    friend class MLiveImage;
    friend class MLiveImagePrivate;
};

#endif
