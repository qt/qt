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

#include "qdeclarativeitemsmodule_p.h"

#include <QtGui/qaction.h>
#include <QtGui/qvalidator.h>
#include <QtGui/qgraphicseffect.h>

#include "qdeclarativeevents_p_p.h"
#include "qdeclarativeeffects_p.h"
#include "qdeclarativescalegrid_p_p.h"
#include "qdeclarativeanimatedimage_p.h"
#include "qdeclarativeborderimage_p.h"
#include "qdeclarativepositioners_p.h"
#include "qdeclarativemousearea_p.h"
#include "qdeclarativeflickable_p.h"
#include "qdeclarativeflickable_p_p.h"
#include "qdeclarativeflipable_p.h"
#include "qdeclarativefocuspanel_p.h"
#include "qdeclarativefocusscope_p.h"
#include "qdeclarativegraphicsobjectcontainer_p.h"
#include "qdeclarativegridview_p.h"
#include "qdeclarativeimage_p.h"
#include "qdeclarativeitem_p.h"
#include "qdeclarativelayoutitem_p.h"
#include "qdeclarativelistview_p.h"
#include "qdeclarativeloader_p.h"
#include "qdeclarativemousearea_p.h"
#include "qdeclarativeparticles_p.h"
#include "qdeclarativepath_p.h"
#include "qdeclarativepathview_p.h"
#include "qdeclarativerectangle_p.h"
#include "qdeclarativerepeater_p.h"
#include "qdeclarativetext_p.h"
#include "qdeclarativetextedit_p.h"
#include "qdeclarativetextinput_p.h"
#include "qdeclarativevisualitemmodel_p.h"
#ifdef QT_WEBKIT_LIB
#include "qdeclarativewebview_p.h"
#include "qdeclarativewebview_p_p.h"
#endif
#include "qdeclarativeanchors_p.h"

void QDeclarativeItemModule::defineModule()
{
    QML_REGISTER_TYPE(Qt,4,6,AnimatedImage,QDeclarativeAnimatedImage);
    QML_REGISTER_TYPE(Qt,4,6,Blur,QGraphicsBlurEffect);
    QML_REGISTER_TYPE(Qt,4,6,BorderImage,QDeclarativeBorderImage);
    QML_REGISTER_TYPE(Qt,4,6,Colorize,QGraphicsColorizeEffect);
    QML_REGISTER_TYPE(Qt,4,6,Column,QDeclarativeColumn);
    QML_REGISTER_TYPE(Qt,4,6,Drag,QDeclarativeDrag);
    QML_REGISTER_TYPE(Qt,4,6,DropShadow,QGraphicsDropShadowEffect);
    QML_REGISTER_TYPE(Qt,4,6,Flickable,QDeclarativeFlickable);
    QML_REGISTER_TYPE(Qt,4,6,Flipable,QDeclarativeFlipable);
    QML_REGISTER_TYPE(Qt,4,6,Flow,QDeclarativeFlow);
    QML_REGISTER_TYPE(Qt,4,6,FocusPanel,QDeclarativeFocusPanel);
    QML_REGISTER_TYPE(Qt,4,6,FocusScope,QDeclarativeFocusScope);
    QML_REGISTER_TYPE(Qt,4,6,Gradient,QDeclarativeGradient);
    QML_REGISTER_TYPE(Qt,4,6,GradientStop,QDeclarativeGradientStop);
    QML_REGISTER_TYPE(Qt,4,6,GraphicsObjectContainer,QDeclarativeGraphicsObjectContainer);
    QML_REGISTER_TYPE(Qt,4,6,Grid,QDeclarativeGrid);
    QML_REGISTER_TYPE(Qt,4,6,GridView,QDeclarativeGridView);
    QML_REGISTER_TYPE(Qt,4,6,Image,QDeclarativeImage);
    QML_REGISTER_TYPE(Qt,4,6,Item,QDeclarativeItem);
    QML_REGISTER_TYPE(Qt,4,6,KeyNavigation,QDeclarativeKeyNavigationAttached);
    QML_REGISTER_TYPE(Qt,4,6,Keys,QDeclarativeKeysAttached);
    QML_REGISTER_TYPE(Qt,4,6,LayoutItem,QDeclarativeLayoutItem);
    QML_REGISTER_TYPE(Qt,4,6,ListView,QDeclarativeListView);
    QML_REGISTER_TYPE(Qt,4,6,Loader,QDeclarativeLoader);
    QML_REGISTER_TYPE(Qt,4,6,MouseRegion,QDeclarativeMouseArea);
    QML_REGISTER_TYPE(Qt,4,6,MouseArea,QDeclarativeMouseArea);
    QML_REGISTER_TYPE(Qt,4,6,Opacity,QGraphicsOpacityEffect);
    QML_REGISTER_TYPE(Qt,4,6,ParticleMotion,QDeclarativeParticleMotion);
    QML_REGISTER_TYPE(Qt,4,6,ParticleMotionGravity,QDeclarativeParticleMotionGravity);
    QML_REGISTER_TYPE(Qt,4,6,ParticleMotionLinear,QDeclarativeParticleMotionLinear);
    QML_REGISTER_TYPE(Qt,4,6,ParticleMotionWander,QDeclarativeParticleMotionWander);
    QML_REGISTER_TYPE(Qt,4,6,Particles,QDeclarativeParticles);
    QML_REGISTER_TYPE(Qt,4,6,Path,QDeclarativePath);
    QML_REGISTER_TYPE(Qt,4,6,PathAttribute,QDeclarativePathAttribute);
    QML_REGISTER_TYPE(Qt,4,6,PathCubic,QDeclarativePathCubic);
    QML_REGISTER_TYPE(Qt,4,6,PathLine,QDeclarativePathLine);
    QML_REGISTER_TYPE(Qt,4,6,PathPercent,QDeclarativePathPercent);
    QML_REGISTER_TYPE(Qt,4,6,PathQuad,QDeclarativePathQuad);
    QML_REGISTER_TYPE(Qt,4,6,PathView,QDeclarativePathView);
    QML_REGISTER_TYPE(Qt,4,6,Pen,QDeclarativePen);
    QML_REGISTER_TYPE(Qt,4,6,QIntValidator,QIntValidator);
#if (QT_VERSION >= QT_VERSION_CHECK(4,7,0))
    QML_REGISTER_TYPE(Qt,4,7,QDoubleValidator,QDoubleValidator);
    QML_REGISTER_TYPE(Qt,4,7,QRegExpValidator,QRegExpValidator);
#endif
    QML_REGISTER_TYPE(Qt,4,6,Rectangle,QDeclarativeRectangle);
    QML_REGISTER_TYPE(Qt,4,6,Repeater,QDeclarativeRepeater);
    QML_REGISTER_TYPE(Qt,4,6,Rotation,QGraphicsRotation);
    QML_REGISTER_TYPE(Qt,4,6,Row,QDeclarativeRow);
    QML_REGISTER_TYPE(Qt,4,6,Scale,QGraphicsScale);
    QML_REGISTER_TYPE(Qt,4,6,Text,QDeclarativeText);
    QML_REGISTER_TYPE(Qt,4,6,TextEdit,QDeclarativeTextEdit);
    QML_REGISTER_TYPE(Qt,4,6,TextInput,QDeclarativeTextInput);
    QML_REGISTER_TYPE(Qt,4,6,ViewSection,QDeclarativeViewSection);
    QML_REGISTER_TYPE(Qt,4,6,VisibleArea,QDeclarativeFlickableVisibleArea);
    QML_REGISTER_TYPE(Qt,4,6,VisualDataModel,QDeclarativeVisualDataModel);
    QML_REGISTER_TYPE(Qt,4,6,VisualItemModel,QDeclarativeVisualItemModel);
#ifdef QT_WEBKIT_LIB
    QML_REGISTER_TYPE(Qt,4,6,WebView,QDeclarativeWebView);
#endif

    QML_REGISTER_NOCREATE_TYPE(QDeclarativeAnchors);
    QML_REGISTER_NOCREATE_TYPE(QGraphicsEffect);
    QML_REGISTER_NOCREATE_TYPE(QDeclarativeKeyEvent);
    QML_REGISTER_NOCREATE_TYPE(QDeclarativeMouseEvent);
    QML_REGISTER_NOCREATE_TYPE(QGraphicsObject);
    QML_REGISTER_NOCREATE_TYPE(QGraphicsTransform);
    QML_REGISTER_NOCREATE_TYPE(QDeclarativePathElement);
    QML_REGISTER_NOCREATE_TYPE(QDeclarativeCurve);
    QML_REGISTER_NOCREATE_TYPE(QDeclarativeScaleGrid);
    QML_REGISTER_NOCREATE_TYPE(QValidator);
    QML_REGISTER_NOCREATE_TYPE(QDeclarativeVisualModel);
    QML_REGISTER_NOCREATE_TYPE(QAction);
#ifdef QT_WEBKIT_LIB
    QML_REGISTER_NOCREATE_TYPE(QDeclarativeWebSettings);
#endif
}
