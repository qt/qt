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

#include "qmlgraphicsitemsmodule_p.h"

#include <QtGui/qaction.h>
#include <QtGui/qvalidator.h>
#include <QtGui/qgraphicseffect.h>

#include "qmlgraphicsevents_p_p.h"
#include "qmlgraphicseffects_p.h"
#include "qmlgraphicsscalegrid_p_p.h"
#include "qmlgraphicsanimatedimage_p.h"
#include "qmlgraphicsborderimage_p.h"
#include "qmlgraphicspositioners_p.h"
#include "qmlgraphicsmousearea_p.h"
#include "qmlgraphicsflickable_p.h"
#include "qmlgraphicsflickable_p_p.h"
#include "qmlgraphicsflipable_p.h"
#include "qmlgraphicsfocuspanel_p.h"
#include "qmlgraphicsfocusscope_p.h"
#include "qmlgraphicsgraphicsobjectcontainer_p.h"
#include "qmlgraphicsgridview_p.h"
#include "qmlgraphicsimage_p.h"
#include "qmlgraphicsitem_p.h"
#include "qmlgraphicslayoutitem_p.h"
#include "qmlgraphicslistview_p.h"
#include "qmlgraphicsloader_p.h"
#include "qmlgraphicsmousearea_p.h"
#include "qmlgraphicsparticles_p.h"
#include "qmlgraphicspath_p.h"
#include "qmlgraphicspathview_p.h"
#include "qmlgraphicsrectangle_p.h"
#include "qmlgraphicsrepeater_p.h"
#include "qmlgraphicstext_p.h"
#include "qmlgraphicstextedit_p.h"
#include "qmlgraphicstextinput_p.h"
#include "qmlgraphicsvisualitemmodel_p.h"
#ifdef QT_WEBKIT_LIB
#include "qmlgraphicswebview_p.h"
#include "qmlgraphicswebview_p_p.h"
#endif
#include "qmlgraphicsanchors_p.h"

#define QML_REGISTER_TYPE(URI,VMAJ,VMIN,TYPE,CLASS) \
            qmlRegisterType<CLASS>(#URI, VMAJ, VMIN, #TYPE, #CLASS)

#define QML_REGISTER_NOCREATE_TYPE(CLASS) \
            qmlRegisterType<CLASS>(#CLASS)

void QmlGraphicsItemModule::defineModule()
{
    QML_REGISTER_TYPE(Qt,4,6,AnimatedImage,QmlGraphicsAnimatedImage);
    QML_REGISTER_TYPE(Qt,4,6,Blur,QGraphicsBlurEffect);
    QML_REGISTER_TYPE(Qt,4,6,BorderImage,QmlGraphicsBorderImage);
    QML_REGISTER_TYPE(Qt,4,6,Colorize,QGraphicsColorizeEffect);
    QML_REGISTER_TYPE(Qt,4,6,Column,QmlGraphicsColumn);
    QML_REGISTER_TYPE(Qt,4,6,Drag,QmlGraphicsDrag);
    QML_REGISTER_TYPE(Qt,4,6,DropShadow,QGraphicsDropShadowEffect);
    QML_REGISTER_TYPE(Qt,4,6,Flickable,QmlGraphicsFlickable);
    QML_REGISTER_TYPE(Qt,4,6,Flipable,QmlGraphicsFlipable);
    QML_REGISTER_TYPE(Qt,4,6,Flow,QmlGraphicsFlow);
    QML_REGISTER_TYPE(Qt,4,6,FocusPanel,QmlGraphicsFocusPanel);
    QML_REGISTER_TYPE(Qt,4,6,FocusScope,QmlGraphicsFocusScope);
    QML_REGISTER_TYPE(Qt,4,6,Gradient,QmlGraphicsGradient);
    QML_REGISTER_TYPE(Qt,4,6,GradientStop,QmlGraphicsGradientStop);
    QML_REGISTER_TYPE(Qt,4,6,GraphicsObjectContainer,QmlGraphicsGraphicsObjectContainer);
    QML_REGISTER_TYPE(Qt,4,6,Grid,QmlGraphicsGrid);
    QML_REGISTER_TYPE(Qt,4,6,GridView,QmlGraphicsGridView);
    QML_REGISTER_TYPE(Qt,4,6,Image,QmlGraphicsImage);
    QML_REGISTER_TYPE(Qt,4,6,Item,QmlGraphicsItem);
    QML_REGISTER_TYPE(Qt,4,6,KeyNavigation,QmlGraphicsKeyNavigationAttached);
    QML_REGISTER_TYPE(Qt,4,6,Keys,QmlGraphicsKeysAttached);
    QML_REGISTER_TYPE(Qt,4,6,LayoutItem,QmlGraphicsLayoutItem);
    QML_REGISTER_TYPE(Qt,4,6,ListView,QmlGraphicsListView);
    QML_REGISTER_TYPE(Qt,4,6,Loader,QmlGraphicsLoader);
    QML_REGISTER_TYPE(Qt,4,6,MouseRegion,QmlGraphicsMouseArea);
    QML_REGISTER_TYPE(Qt,4,6,MouseArea,QmlGraphicsMouseArea);
    QML_REGISTER_TYPE(Qt,4,6,Opacity,QGraphicsOpacityEffect);
    QML_REGISTER_TYPE(Qt,4,6,ParticleMotion,QmlGraphicsParticleMotion);
    QML_REGISTER_TYPE(Qt,4,6,ParticleMotionGravity,QmlGraphicsParticleMotionGravity);
    QML_REGISTER_TYPE(Qt,4,6,ParticleMotionLinear,QmlGraphicsParticleMotionLinear);
    QML_REGISTER_TYPE(Qt,4,6,ParticleMotionWander,QmlGraphicsParticleMotionWander);
    QML_REGISTER_TYPE(Qt,4,6,Particles,QmlGraphicsParticles);
    QML_REGISTER_TYPE(Qt,4,6,Path,QmlGraphicsPath);
    QML_REGISTER_TYPE(Qt,4,6,PathAttribute,QmlGraphicsPathAttribute);
    QML_REGISTER_TYPE(Qt,4,6,PathCubic,QmlGraphicsPathCubic);
    QML_REGISTER_TYPE(Qt,4,6,PathLine,QmlGraphicsPathLine);
    QML_REGISTER_TYPE(Qt,4,6,PathPercent,QmlGraphicsPathPercent);
    QML_REGISTER_TYPE(Qt,4,6,PathQuad,QmlGraphicsPathQuad);
    QML_REGISTER_TYPE(Qt,4,6,PathView,QmlGraphicsPathView);
    QML_REGISTER_TYPE(Qt,4,6,Pen,QmlGraphicsPen);
    QML_REGISTER_TYPE(Qt,4,6,QIntValidator,QIntValidator);
#if (QT_VERSION >= QT_VERSION_CHECK(4,7,0))
    QML_REGISTER_TYPE(Qt,4,7,QDoubleValidator,QDoubleValidator);
    QML_REGISTER_TYPE(Qt,4,7,QRegExpValidator,QRegExpValidator);
#endif
    QML_REGISTER_TYPE(Qt,4,6,Rectangle,QmlGraphicsRectangle);
    QML_REGISTER_TYPE(Qt,4,6,Repeater,QmlGraphicsRepeater);
    QML_REGISTER_TYPE(Qt,4,6,Rotation,QGraphicsRotation);
    QML_REGISTER_TYPE(Qt,4,6,Row,QmlGraphicsRow);
    QML_REGISTER_TYPE(Qt,4,6,Scale,QGraphicsScale);
    QML_REGISTER_TYPE(Qt,4,6,Text,QmlGraphicsText);
    QML_REGISTER_TYPE(Qt,4,6,TextEdit,QmlGraphicsTextEdit);
    QML_REGISTER_TYPE(Qt,4,6,TextInput,QmlGraphicsTextInput);
    QML_REGISTER_TYPE(Qt,4,6,ViewSection,QmlGraphicsViewSection);
    QML_REGISTER_TYPE(Qt,4,6,VisibleArea,QmlGraphicsFlickableVisibleArea);
    QML_REGISTER_TYPE(Qt,4,6,VisualDataModel,QmlGraphicsVisualDataModel);
    QML_REGISTER_TYPE(Qt,4,6,VisualItemModel,QmlGraphicsVisualItemModel);
#ifdef QT_WEBKIT_LIB
    QML_REGISTER_TYPE(Qt,4,6,WebView,QmlGraphicsWebView);
#endif

    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsAnchors);
    QML_REGISTER_NOCREATE_TYPE(QGraphicsEffect);
    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsKeyEvent);
    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsMouseEvent);
    QML_REGISTER_NOCREATE_TYPE(QGraphicsObject);
    QML_REGISTER_NOCREATE_TYPE(QGraphicsTransform);
    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsPathElement);
    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsCurve);
    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsScaleGrid);
    QML_REGISTER_NOCREATE_TYPE(QValidator);
    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsVisualModel);
    QML_REGISTER_NOCREATE_TYPE(QAction);
#ifdef QT_WEBKIT_LIB
    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsWebSettings);
#endif
}
