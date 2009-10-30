/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qml.h>
#include <qfxglobal.h>
#include <QtGui/qgraphicseffect.h>

QML_DECLARE_TYPE(QGraphicsEffect)
QML_DEFINE_NOCREATE_TYPE(QGraphicsEffect)

QML_DECLARE_TYPE(QGraphicsBlurEffect)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Blur,QGraphicsBlurEffect)

/*!
    \qmlclass Blur QGraphicsBlurEffect
    \brief The Blur object provides a blur effect.

    A blur effect blurs the source item. This effect is useful for reducing details,
    such as when the source loses focus and you want to draw attention to other
    elements. The level of detail can be modified using the blurRadius property.
    Use blurHint to choose the quality or performance blur hints.

    By default, the blur radius is 5 pixels.

    \img graphicseffect-blur.png
*/

/*!
    \qmlproperty real Blur::blurRadius

    blurRadius controls how blurry an item will appear.
    Using a smaller radius results in a sharper appearance, whereas a bigger
    radius results in a more blurred appearance.

    By default, the blur radius is 5 pixels.
*/
/*!
    \qmlproperty enumeration Blur::blurHint

    Use the Qt.PerformanceHint hint to say that you want a faster blur,
    and the Qt.QualityHint hint to say that you prefer a higher quality blur.

    When animating the blur radius it's recommended to use Qt.PerformanceHint.

    By default, the blur hint is Qt.PerformanceHint.
*/

QML_DECLARE_TYPE(QGraphicsGrayscaleEffect)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Grayscale,QGraphicsGrayscaleEffect)

/*!
    \qmlclass Grayscale QGraphicsGrayscaleEffect
    \brief The Grayscale object provides a grayscale effect.

    A grayscale effect renders the source item in shades of gray.

    \img graphicseffect-grayscale.png
*/

/*!
    \qmlproperty real Grayscale::strength

    To what extent the source item is "grayed". A strength of 0.0 is equal to no effect,
    while 1.0 means full grayscale. By default, the strength is 1.0.
*/

QML_DECLARE_TYPE(QGraphicsColorizeEffect)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Colorize,QGraphicsColorizeEffect)

/*!
    \qmlclass Colorize QGraphicsColorizeEffect
    \brief The Colorize object provides a colorize effect.

    A colorize effect renders the source item with a tint of its color.

    By default, the color is light blue.

    \img graphicseffect-colorize.png
*/

/*!
    \qmlproperty color Colorize::color
    The color of the effect.

    By default, the color is light blue.
*/

/*!
    \qmlproperty real Colorize::strength

    To what extent the source item is "colored". A strength of 0.0 is equal to no effect,
    while 1.0 means full colorization. By default, the strength is 1.0.
*/

QML_DECLARE_TYPE(QGraphicsPixelizeEffect)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Pixelize,QGraphicsPixelizeEffect)

/*!
    \qmlclass Pixelize QGraphicsPixelizeEffect
    \brief The Pixelize object provides a pixelize effect.

    A pixelize effect renders the source item in lower resolution. The resolution
    can be modified using the pixelSize property.

    By default, the pixel size is 3.

    \img graphicseffect-pixelize.png
*/

/*!
    \qmlproperty int Pixelize::pixelSize
    The size of a pixel in the effect.

    Setting the pixel size to 2 means two pixels in the source item will be used to
    represent one pixel in the output. Using a bigger size results in lower resolution.

    By default, the pixel size is 3.
*/


QML_DECLARE_TYPE(QGraphicsDropShadowEffect)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,DropShadow,QGraphicsDropShadowEffect)

/*!
    \qmlclass DropShadow QGraphicsDropShadowEffect
    \brief The DropShadow object provides a drop shadow effect.

    A drop shadow effect renders the source item with a drop shadow. The color of
    the drop shadow can be modified using the color property. The drop
    shadow offset can be modified using the xOffset and yOffset properties and the blur
    radius of the drop shadow can be changed with the blurRadius property.

    By default, the drop shadow is a semi-transparent dark gray shadow,
    blurred with a radius of 1 at an offset of 8 pixels towards the lower right.

    \img graphicseffect-drop-shadow.png
*/

/*!
    \qmlproperty real DropShadow::xOffset
    \qmlproperty real DropShadow::yOffset
    The shadow offset in pixels.

    By default, xOffset and yOffset are 8 pixels.
*/

/*!
    \qmlproperty real DropShadow::blurRadius
    The blur radius in pixels of the drop shadow.

    Using a smaller radius results in a sharper shadow, whereas using a bigger
    radius results in a more blurred shadow.

    By default, the blur radius is 1 pixel.
*/

/*!
    \qmlproperty color DropShadow::color
    The color of the drop shadow.

    By default, the drop color is a semi-transparent dark gray.
*/

QML_DECLARE_TYPE(QGraphicsOpacityEffect)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Opacity,QGraphicsOpacityEffect)

/*!
    \qmlclass Opacity QGraphicsOpacityEffect
    \brief The Opacity object provides an opacity effect.

    An opacity effect renders the source with an opacity. This effect is useful
    for making the source semi-transparent, similar to a fade-in/fade-out
    sequence. The opacity can be modified using the opacity property.

    By default, the opacity is 0.7.

    \img graphicseffect-opacity.png
*/

/*!
    \qmlproperty real Opacity::opacity
    This property specifies how opaque an item should appear.

    The value should be in the range of 0.0 to 1.0, where 0.0 is
    fully transparent and 1.0 is fully opaque.

    By default, the opacity is 0.7.
*/

QML_DECLARE_TYPE(QGraphicsBloomEffect)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Bloom,QGraphicsBloomEffect)

/*!
    \qmlclass Bloom QGraphicsBloomEffect
    \brief The Bloom object provides a bloom/glow effect.

    A bloom/glow effect adds fringes of light around bright areas in the source item.

    \img graphicseffect-bloom.png
*/

/*!
    \qmlproperty real Bloom::blurRadius
    The blur radius in pixels of the effect.

    Using a smaller radius results in a sharper appearance, whereas a bigger
    radius results in a more blurred appearance.

    By default, the blur radius is 5 pixels.
*/

/*!
    \qmlproperty enumeration Bloom::blurHint

    Use the Qt.PerformanceHint hint to say that you want a faster blur,
    and the Qt.QualityHint hint to say that you prefer a higher quality blur.

    When animating the blur radius it's recommended to use Qt.PerformanceHint.

    By default, the blur hint is Qt.PerformanceHint.
*/

/*!
    \qmlproperty int Bloom::brightness
    This property specifies how bright the glow should appear.

    The value should be in the range of 0 to 255, where 0 is dark
    and 255 is bright.

    By default, the brightness is 70.
*/

/*!
    \qmlproperty real Bloom::strength
    The strength of the glow.

    A strength of 0.0 is equal to no effect, while 1.0 means maximum glow.

    By default, the strength is 0.7.
*/

