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

#ifndef QMEEGOLIVEIMAGE_H
#define QMEEGOLIVEIMAGE_H

#include <QImage>

class QMeeGoLivePixmap;
class QMeeGoLiveImagePrivate;

//! A streamable QImage subclass.
/*!
*/

class QMeeGoLiveImage : public QImage
{
public:
    //! Format specifier.
    /*! 
     Used to specify the format of the underlying image data for QMeeGoLiveImage. 
    */
    enum Format {
        Format_ARGB32_Premultiplied //! 32bit, AARRGGBB format. The typical Qt format.
    };
    
    //! Locks the access to the image. 
    /*! 
     All drawing/access to the underlying image data needs to happen between 
     ::lock() and ::unlock() pairs.
     */
    void lock(int buffer = 0);
    
    //! Unlocks the access to the image. 
    /*! 
     All drawing/access to the underlying image data needs to happen between 
     ::lock() and ::unlock() pairs.
     */
    void release(int buffer = 0);
    
    //! Destroys the image.
    /*!
      It's a mistake to destroy an image before destroying all the QMeeGoLivePixmaps
      built on top of it. You should first destroy all the QMeeGoLivePixmaps.
     */
    virtual ~QMeeGoLiveImage();
    
    //! Creates and returns a new live image with the given parameters.
    /*!
     The new image is created with the given width w and the given height h. 
     The format specifies the color format used by the image. Optionally, a 
     number of buffers can be specfied for a stream-like behavior.
     */
    static QMeeGoLiveImage* liveImageWithSize(int w, int h, Format format, int buffers = 1);

private:
    QMeeGoLiveImage(int w, int h); //! Private bits.
    Q_DISABLE_COPY(QMeeGoLiveImage)
    Q_DECLARE_PRIVATE(QMeeGoLiveImage)

protected:
    QScopedPointer<QMeeGoLiveImagePrivate> d_ptr;

    friend class QMeeGoLivePixmap;
    friend class QMeeGoLivePixmapPrivate;
};

#endif
