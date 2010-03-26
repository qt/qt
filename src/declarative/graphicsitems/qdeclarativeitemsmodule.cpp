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
#include "qdeclarativepath_p.h"
#include "qdeclarativepathview_p.h"
#include "qdeclarativerectangle_p.h"
#include "qdeclarativerepeater_p.h"
#include "qdeclarativetranslate_p.h"
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
    qmlRegisterType<QDeclarativeAnimatedImage>("Qt",4,6,"AnimatedImage");
    qmlRegisterType<QGraphicsBlurEffect>("Qt",4,6,"Blur");
    qmlRegisterType<QDeclarativeBorderImage>("Qt",4,6,"BorderImage");
    qmlRegisterType<QGraphicsColorizeEffect>("Qt",4,6,"Colorize");
    qmlRegisterType<QDeclarativeColumn>("Qt",4,6,"Column");
    qmlRegisterType<QDeclarativeDrag>("Qt",4,6,"Drag");
    qmlRegisterType<QGraphicsDropShadowEffect>("Qt",4,6,"DropShadow");
    qmlRegisterType<QDeclarativeFlickable>("Qt",4,6,"Flickable");
    qmlRegisterType<QDeclarativeFlipable>("Qt",4,6,"Flipable");
    qmlRegisterType<QDeclarativeFlow>("Qt",4,6,"Flow");
    qmlRegisterType<QDeclarativeFocusPanel>("Qt",4,6,"FocusPanel");
    qmlRegisterType<QDeclarativeFocusScope>("Qt",4,6,"FocusScope");
    qmlRegisterType<QDeclarativeGradient>("Qt",4,6,"Gradient");
    qmlRegisterType<QDeclarativeGradientStop>("Qt",4,6,"GradientStop");
    qmlRegisterType<QDeclarativeGraphicsObjectContainer>("Qt",4,6,"GraphicsObjectContainer");
    qmlRegisterType<QDeclarativeGrid>("Qt",4,6,"Grid");
    qmlRegisterType<QDeclarativeGridView>("Qt",4,6,"GridView");
    qmlRegisterType<QDeclarativeImage>("Qt",4,6,"Image");
    qmlRegisterType<QDeclarativeItem>("Qt",4,6,"Item");
    qmlRegisterType<QDeclarativeKeyNavigationAttached>("Qt",4,6,"KeyNavigation");
    qmlRegisterType<QDeclarativeKeysAttached>("Qt",4,6,"Keys");
    qmlRegisterType<QDeclarativeLayoutItem>("Qt",4,6,"LayoutItem");
    qmlRegisterType<QDeclarativeListView>("Qt",4,6,"ListView");
    qmlRegisterType<QDeclarativeLoader>("Qt",4,6,"Loader");
    qmlRegisterType<QDeclarativeMouseArea>("Qt",4,6,"MouseArea");
    qmlRegisterType<QGraphicsOpacityEffect>("Qt",4,6,"Opacity");
    qmlRegisterType<QDeclarativePath>("Qt",4,6,"Path");
    qmlRegisterType<QDeclarativePathAttribute>("Qt",4,6,"PathAttribute");
    qmlRegisterType<QDeclarativePathCubic>("Qt",4,6,"PathCubic");
    qmlRegisterType<QDeclarativePathLine>("Qt",4,6,"PathLine");
    qmlRegisterType<QDeclarativePathPercent>("Qt",4,6,"PathPercent");
    qmlRegisterType<QDeclarativePathQuad>("Qt",4,6,"PathQuad");
    qmlRegisterType<QDeclarativePathView>("Qt",4,6,"PathView");
    qmlRegisterType<QIntValidator>("Qt",4,6,"IntValidator");
#if (QT_VERSION >= QT_VERSION_CHECK(4,7,0))
    qmlRegisterType<QDoubleValidator>("Qt",4,7,"DoubleValidator");
    qmlRegisterType<QRegExpValidator>("Qt",4,7,"RegExpValidator");
#endif
    qmlRegisterType<QDeclarativeRectangle>("Qt",4,6,"Rectangle");
    qmlRegisterType<QDeclarativeRepeater>("Qt",4,6,"Repeater");
    qmlRegisterType<QGraphicsRotation>("Qt",4,6,"Rotation");
    qmlRegisterType<QDeclarativeRow>("Qt",4,6,"Row");
    qmlRegisterType<QDeclarativeTranslate>("Qt",4,6,"Translate");
    qmlRegisterType<QGraphicsScale>("Qt",4,6,"Scale");
    qmlRegisterType<QDeclarativeText>("Qt",4,6,"Text");
    qmlRegisterType<QDeclarativeTextEdit>("Qt",4,6,"TextEdit");
    qmlRegisterType<QDeclarativeTextInput>("Qt",4,6,"TextInput");
    qmlRegisterType<QDeclarativeViewSection>("Qt",4,6,"ViewSection");
    qmlRegisterType<QDeclarativeFlickableVisibleArea>("Qt",4,6,"VisibleArea");
    qmlRegisterType<QDeclarativeVisualDataModel>("Qt",4,6,"VisualDataModel");
    qmlRegisterType<QDeclarativeVisualItemModel>("Qt",4,6,"VisualItemModel");

    qmlRegisterType<QDeclarativeAnchors>();
    qmlRegisterType<QGraphicsEffect>();
    qmlRegisterType<QDeclarativeKeyEvent>();
    qmlRegisterType<QDeclarativeMouseEvent>();
    qmlRegisterType<QGraphicsObject>();
    qmlRegisterType<QGraphicsWidget>();
    qmlRegisterType<QGraphicsTransform>();
    qmlRegisterType<QDeclarativePathElement>();
    qmlRegisterType<QDeclarativeCurve>();
    qmlRegisterType<QDeclarativeScaleGrid>();
    qmlRegisterType<QValidator>();
    qmlRegisterType<QDeclarativeVisualModel>();
    qmlRegisterType<QAction>();
    qmlRegisterType<QDeclarativePen>();
#ifdef QT_WEBKIT_LIB
    qmlRegisterType<QDeclarativeWebSettings>();
#endif
}
