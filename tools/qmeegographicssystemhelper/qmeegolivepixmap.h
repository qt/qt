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

#ifndef QMEEGOLIVEPIXMAP_H
#define QMEEGOLIVEPIXMAP_H

#include <QtGui/QPixmap>
#include "QtMeeGoGraphicsSystemHelper/qmeegofencesync.h"

class QMeeGoLivePixmapPrivate;
class QSharedMemory;
class QImage;

//! A pixmap representing streamed content.
/*!
*/

class Q_DECL_EXPORT QMeeGoLivePixmap : public QPixmap
{
public:
    enum Format {
        Format_RGB16,               //! 16bit, 5-6-5 RGB format.
        Format_ARGB32_Premultiplied //! 32bit, AARRGGBB format. The typical Qt format.
    };
    
    //! Creates and returns a new live pixmap with the given parameters.
    /*!
     The new pixmap is created with the given width w and the given height h. 
     The format specifies the color format used by the pixmap.
    */
    static QMeeGoLivePixmap* livePixmapWithSize(int w, int h, Format format);
    
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

    //! Locks the access to the pixmap. 
    /*! 
     The returned image can be used for direct access.
     You can optionally specify a fence sync to wait upon before unlocking. When
     you specify a fence sync, you can be sure that this function will return only
     when the previsouly set QMeeGoFenceSync synchronization point has been executed/passed
     by the GL processing pipeline.
     */
    QImage* lock(QMeeGoFenceSync *fenceSync = NULL);
    
    //! Unlocks the access to the pixmap. 
    /*! 
     */
    void release(QImage *img);
    
    //! Destroys the QMeeGoLivePixmap.
    /*! 
    */
    virtual ~QMeeGoLivePixmap();

private:
    QMeeGoLivePixmap(QPixmapData *p);
    Q_DISABLE_COPY(QMeeGoLivePixmap)
    Q_DECLARE_PRIVATE(QMeeGoLivePixmap)

protected:
    QScopedPointer<QMeeGoLivePixmapPrivate> d_ptr; //! Private bits.
};

#endif
