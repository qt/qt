/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativeimageprovider.h"

QT_BEGIN_NAMESPACE

class QDeclarativeImageProviderPrivate
{
public:
    QDeclarativeImageProvider::ImageType type;
};

/*!
    \class QDeclarativeImageProvider
    \since 4.7
    \brief The QDeclarativeImageProvider class provides an interface for supporting pixmaps and threaded image requests in QML.

    QDeclarativeImageProvider is used to provide advanced image loading features
    in QML applications. It allows images in QML to be:

    \list
    \o Loaded using QPixmaps rather than actual image files
    \o Loaded asynchronously in a separate thread, if imageType() is \l ImageType::Image
    \endlist

    To specify that an image should be loaded by an image provider, use the
    \bold {"image:"} scheme for the URL source of the image, followed by the 
    identifiers of the image provider and the requested image. For example:
   
    \qml
    Image { source: "image://myimageprovider/image.png" }
    \endqml

    This specifies that the image should be loaded by the image provider named 
    "myimageprovider", and the image to be loaded is named "image.png". The QML engine 
    invokes the appropriate image provider according to the providers that have
    been registered through QDeclarativeEngine::addImageProvider().


    \section2 An example

    Here are two images. Their \c source values indicate they should be loaded by
    an image provider named "colors", and the images to be loaded are "yellow"
    and "red", respectively:

    \snippet examples/declarative/cppextensions/imageprovider/imageprovider-example.qml 0
 
    When these images are loaded by QML, it looks for a matching image provider
    and calls its requestImage() or requestPixmap() method (depending on its
    imageType()) to load the image. The method is called with the \c id 
    parameter set to "yellow" for the first image, and "red" for the second.

    Here is an image provider implementation that can load the images 
    requested by the above QML. This implementation dynamically 
    generates QPixmap images that are filled with the requested color:

    \snippet examples/declarative/cppextensions/imageprovider/imageprovider.cpp 0
    \codeline
    \snippet examples/declarative/cppextensions/imageprovider/imageprovider.cpp 1

    To make this provider accessible to QML, it is registered with the QML engine
    with a "colors" identifier:

    \code
    int main(int argc, char *argv[]) 
    {
        ...

        QDeclarativeEngine engine;
        engine->addImageProvider(QLatin1String("colors"), new ColorPixmapProvider);

        ...
    }
    \endcode

    Now the images can be succesfully loaded in QML:

    \image imageprovider.png

    A complete example is available in Qt's 
    \l {declarative/cppextensions/imageprovider}{examples/declarative/cppextensions/imageprovider}
    directory. Note the example registers the provider via a \l{QDeclarativeExtensionPlugin}{plugin}
    instead of registering it in the application \c main() function as shown above.


    \section2 Asynchronous image loading

    Image providers that support QImage loading automatically include support
    for asychronous loading of images. To enable asynchronous loading for an
    \l Image source, set \l Image::asynchronous to \c true. When this is enabled, 
    the image request to the provider is run in a low priority thread,
    allowing image loading to be executed in the background, and reducing the
    performance impact on the user interface.

    Asynchronous loading is not supported for image providers that provide
    QPixmap rather than QImage values, as pixmaps can only be created in the
    main thread. In this case, if \l {Image::}{asynchronous} is set to 
    \c true, the value is ignored and the image is loaded
    synchronously.
   
    \sa QDeclarativeEngine::addImageProvider()
*/

/*!
    \enum QDeclarativeImageProvider::ImageType

    Defines the type of image supported by this image provider.

    \value Image The Image Provider provides QImage images. The 
        requestImage() method will be called for all image requests.
    \value Pixmap The Image Provider provides QPixmap images. The 
        requestPixmap() method will be called for all image requests.
*/

/*!
    Creates an image provider that will provide images of the given \a type.
*/
QDeclarativeImageProvider::QDeclarativeImageProvider(ImageType type)
    : d(new QDeclarativeImageProviderPrivate)
{
    d->type = type;
}

/*!
   \internal
*/
QDeclarativeImageProvider::~QDeclarativeImageProvider()
{
    delete d;
}

/*!
    Returns the image type supported by this provider.
*/
QDeclarativeImageProvider::ImageType QDeclarativeImageProvider::imageType() const
{
    return d->type;
}

/*!
    Implement this method to return the image with \a id. The default 
    implementation returns an empty image.

    The \a requestedSize corresponds to the \l {Image::sourceSize} requested by
    an Image element. If \a requestedSize is a valid size, the image
    returned should be of that size.

    In all cases, \a size must be set to the original size of the image. This
    is used to set the \l {Item::}{width} and \l {Item::}{height} of image
    elements that should be automatically sized to the loaded image.

    \note this method may be called by multiple threads, so ensure the
    implementation of this method is reentrant.
*/
QImage QDeclarativeImageProvider::requestImage(const QString &id, QSize *size, const QSize& requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);
    if (d->type == Image)
        qWarning("ImageProvider supports Image type but has not implemented requestImage()");
    return QImage();
}

/*!
    Implement this method to return the pixmap with \a id. The default
    implementation returns an empty pixmap.

    The \a requestedSize corresponds to the \l {Image::sourceSize} requested by
    an Image element. If \a requestedSize is a valid size, the image
    returned should be of that size.

    In all cases, \a size must be set to the original size of the image. This
    is used to set the \l {Item::}{width} and \l {Item::}{height} of image
    elements that should be automatically sized to the loaded image.
*/
QPixmap QDeclarativeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize& requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);
    if (d->type == Pixmap)
        qWarning("ImageProvider supports Pixmap type but has not implemented requestPixmap()");
    return QPixmap();
}

QT_END_NAMESPACE

