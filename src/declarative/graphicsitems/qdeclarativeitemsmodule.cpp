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

#include "private/qdeclarativeitemsmodule_p.h"

#include <QtGui/qaction.h>
#include <QtGui/qvalidator.h>
#include <QtGui/qgraphicseffect.h>

#include "private/qdeclarativeevents_p_p.h"
#include "private/qdeclarativescalegrid_p_p.h"
#include "private/qdeclarativeanimatedimage_p.h"
#include "private/qdeclarativeborderimage_p.h"
#include "private/qdeclarativepositioners_p.h"
#include "private/qdeclarativemousearea_p.h"
#include "private/qdeclarativeflickable_p.h"
#include "private/qdeclarativeflickable_p_p.h"
#include "private/qdeclarativeflipable_p.h"
#include "private/qdeclarativefocuspanel_p.h"
#include "private/qdeclarativefocusscope_p.h"
#include "private/qdeclarativegridview_p.h"
#include "private/qdeclarativeimage_p.h"
#include "private/qdeclarativeitem_p.h"
#include "private/qdeclarativelayoutitem_p.h"
#include "private/qdeclarativelistview_p.h"
#include "private/qdeclarativeloader_p.h"
#include "private/qdeclarativemousearea_p.h"
#include "private/qdeclarativepath_p.h"
#include "private/qdeclarativepathview_p.h"
#include "private/qdeclarativerectangle_p.h"
#include "private/qdeclarativerepeater_p.h"
#include "private/qdeclarativetranslate_p.h"
#include "private/qdeclarativetext_p.h"
#include "private/qdeclarativetextedit_p.h"
#include "private/qdeclarativetextinput_p.h"
#include "private/qdeclarativevisualitemmodel_p.h"
#include "private/qdeclarativegraphicswidget_p.h"
#ifdef QT_WEBKIT_LIB
#include "private/qdeclarativewebview_p.h"
#include "private/qdeclarativewebview_p_p.h"
#endif
#include "private/qdeclarativeanchors_p.h"

static QDeclarativePrivate::AutoParentResult qgraphicsobject_autoParent(QObject *obj, QObject *parent)
{
    QGraphicsObject* gobj = qobject_cast<QGraphicsObject*>(obj);
    if (!gobj)
        return QDeclarativePrivate::IncompatibleObject;

    QGraphicsObject* gparent = qobject_cast<QGraphicsObject*>(parent);
    if (!gparent)
        return QDeclarativePrivate::IncompatibleParent;

    gobj->setParentItem(gparent);
    return QDeclarativePrivate::Parented;
}

void QDeclarativeItemModule::defineModule()
{
    QDeclarativePrivate::RegisterAutoParent autoparent = { 0, &qgraphicsobject_autoParent };
    QDeclarativePrivate::qmlregister(QDeclarativePrivate::AutoParentRegistration, &autoparent);

#ifdef QT_NO_MOVIE
    qmlRegisterTypeNotAvailable("Qt",4,7,"AnimatedImage",
        qApp->translate("QDeclarativeAnimatedImage","Qt was built without support for QMovie"));
#else
    qmlRegisterType<QDeclarativeAnimatedImage>("Qt",4,7,"AnimatedImage");
#endif
    qmlRegisterType<QDeclarativeBorderImage>("Qt",4,7,"BorderImage");
    qmlRegisterType<QDeclarativeColumn>("Qt",4,7,"Column");
    qmlRegisterType<QDeclarativeDrag>("Qt",4,7,"Drag");
    qmlRegisterType<QDeclarativeFlickable>("Qt",4,7,"Flickable");
    qmlRegisterType<QDeclarativeFlipable>("Qt",4,7,"Flipable");
    qmlRegisterType<QDeclarativeFlow>("Qt",4,7,"Flow");
    qmlRegisterType<QDeclarativeFocusPanel>("Qt",4,7,"FocusPanel");
    qmlRegisterType<QDeclarativeFocusScope>("Qt",4,7,"FocusScope");
    qmlRegisterType<QDeclarativeGradient>("Qt",4,7,"Gradient");
    qmlRegisterType<QDeclarativeGradientStop>("Qt",4,7,"GradientStop");
    qmlRegisterType<QDeclarativeGrid>("Qt",4,7,"Grid");
    qmlRegisterType<QDeclarativeGridView>("Qt",4,7,"GridView");
    qmlRegisterType<QDeclarativeImage>("Qt",4,7,"Image");
    qmlRegisterType<QDeclarativeItem>("Qt",4,7,"Item");
    qmlRegisterType<QDeclarativeLayoutItem>("Qt",4,7,"LayoutItem");
    qmlRegisterType<QDeclarativeListView>("Qt",4,7,"ListView");
    qmlRegisterType<QDeclarativeLoader>("Qt",4,7,"Loader");
    qmlRegisterType<QDeclarativeMouseArea>("Qt",4,7,"MouseArea");
    qmlRegisterType<QDeclarativePath>("Qt",4,7,"Path");
    qmlRegisterType<QDeclarativePathAttribute>("Qt",4,7,"PathAttribute");
    qmlRegisterType<QDeclarativePathCubic>("Qt",4,7,"PathCubic");
    qmlRegisterType<QDeclarativePathLine>("Qt",4,7,"PathLine");
    qmlRegisterType<QDeclarativePathPercent>("Qt",4,7,"PathPercent");
    qmlRegisterType<QDeclarativePathQuad>("Qt",4,7,"PathQuad");
    qmlRegisterType<QDeclarativePathView>("Qt",4,7,"PathView");
#ifndef QT_NO_VALIDATOR
    qmlRegisterType<QIntValidator>("Qt",4,7,"IntValidator");
    qmlRegisterType<QDoubleValidator>("Qt",4,7,"DoubleValidator");
    qmlRegisterType<QRegExpValidator>("Qt",4,7,"RegExpValidator");
#endif
    qmlRegisterType<QDeclarativeRectangle>("Qt",4,7,"Rectangle");
    qmlRegisterType<QDeclarativeRepeater>("Qt",4,7,"Repeater");
    qmlRegisterType<QGraphicsRotation>("Qt",4,7,"Rotation");
    qmlRegisterType<QDeclarativeRow>("Qt",4,7,"Row");
    qmlRegisterType<QDeclarativeTranslate>("Qt",4,7,"Translate");
    qmlRegisterType<QGraphicsScale>("Qt",4,7,"Scale");
    qmlRegisterType<QDeclarativeText>("Qt",4,7,"Text");
    qmlRegisterType<QDeclarativeTextEdit>("Qt",4,7,"TextEdit");
    qmlRegisterType<QDeclarativeTextInput>("Qt",4,7,"TextInput");
    qmlRegisterType<QDeclarativeViewSection>("Qt",4,7,"ViewSection");
    qmlRegisterType<QDeclarativeVisualDataModel>("Qt",4,7,"VisualDataModel");
    qmlRegisterType<QDeclarativeVisualItemModel>("Qt",4,7,"VisualItemModel");

    qmlRegisterType<QDeclarativeAnchors>();
    qmlRegisterType<QDeclarativeKeyEvent>();
    qmlRegisterType<QDeclarativeMouseEvent>();
    qmlRegisterType<QGraphicsObject>();
    qmlRegisterType<QGraphicsWidget>("Qt",4,7,"QGraphicsWidget");
    qmlRegisterExtendedType<QGraphicsWidget,QDeclarativeGraphicsWidget>("Qt",4,7,"QGraphicsWidget");
    qmlRegisterType<QGraphicsTransform>();
    qmlRegisterType<QDeclarativePathElement>();
    qmlRegisterType<QDeclarativeCurve>();
    qmlRegisterType<QDeclarativeScaleGrid>();
#ifndef QT_NO_VALIDATOR
    qmlRegisterType<QValidator>();
#endif
    qmlRegisterType<QDeclarativeVisualModel>();
#ifndef QT_NO_ACTION
    qmlRegisterType<QAction>();
#endif
    qmlRegisterType<QDeclarativePen>();
    qmlRegisterType<QDeclarativeFlickableVisibleArea>();
#ifndef QT_NO_GRAPHICSEFFECT
    qmlRegisterType<QGraphicsEffect>();
#endif

    qmlRegisterUncreatableType<QDeclarativeKeyNavigationAttached>("Qt",4,7,"KeyNavigation",QDeclarativeKeyNavigationAttached::tr("KeyNavigation is only available via attached properties"));
    qmlRegisterUncreatableType<QDeclarativeKeysAttached>("Qt",4,7,"Keys",QDeclarativeKeysAttached::tr("Keys is only available via attached properties"));
}
