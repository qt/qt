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

#include <qdeclarative.h>

#include <QtGui/qgraphicseffect.h>

/*!
    \qmlclass Blur QGraphicsBlurEffect
    \since 4.7
    \brief The Blur object provides a blur effect.

    A blur effect blurs the source item. This effect is useful for reducing details;
    for example, when the a source loses focus and attention should be drawn to other
    elements. Use blurRadius to control the level of detail and blurHint to control
    the quality of the blur.

    By default, the blur radius is 5 pixels.

    \img graphicseffect-blur.png
*/

/*!
    \qmlproperty real Blur::blurRadius

    This controls how blurry an item will appear.

    A smaller radius produces a sharper appearance, and a larger radius produces
    a more blurred appearance.

    The default radius is 5 pixels.
*/
/*!
    \qmlproperty enumeration Blur::blurHint

    Use Qt.PerformanceHint to specify a faster blur or Qt.QualityHint hint
    to specify a higher quality blur.
   
    If the blur radius is animated, it is recommended you use Qt.PerformanceHint.

    The default hint is Qt.PerformanceHint.
*/

/*!
    \qmlclass Colorize QGraphicsColorizeEffect
    \since 4.7
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


/*!
    \qmlclass DropShadow QGraphicsDropShadowEffect
    \since 4.7
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


/*!
    \qmlclass Opacity QGraphicsOpacityEffect
    \since 4.7
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

