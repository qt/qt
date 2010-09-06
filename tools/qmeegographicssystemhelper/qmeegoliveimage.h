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

#ifndef MLIVEIMAGE_H
#define MLIVEIMAGE_H

#include <QImage>

class MLivePixmap;
class MLiveImagePrivate;

//! A streamable QImage subclass.
/*!
*/

class MLiveImage : public QImage
{
public:
    //! Format specifier.
    /*! 
     Used to specify the format of the underlying image data for MLiveImage. 
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
      It's a mistake to destroy an image before destroying all the MLivePixmaps
      built on top of it. You should first destroy all the MLivePixmaps.
     */
    virtual ~MLiveImage();
    
    //! Creates and returns a new live image with the given parameters.
    /*!
     The new image is created with the given width w and the given height h. 
     The format specifies the color format used by the image. Optionally, a 
     number of buffers can be specfied for a stream-like behavior.
     */
    static MLiveImage* liveImageWithSize(int w, int h, Format format, int buffers = 1);

private:
    MLiveImage(int w, int h); //! Private bits.
    Q_DISABLE_COPY(MLiveImage)
    Q_DECLARE_PRIVATE(MLiveImage)

protected:
    QScopedPointer<MLiveImagePrivate> d_ptr;

    friend class MLivePixmap;
    friend class MLivePixmapPrivate;
};

#endif
