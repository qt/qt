// Commit: 2c7cab4172f1acc86fd49345a2847417e162f2c3
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

#include "qsgitemsmodule_p.h"

#include "qsgitem.h"
#include "qsgitem_p.h"
#include "qsgevents_p_p.h"
#include "qsgrectangle_p.h"
#include "qsgfocusscope_p.h"
#include "qsgtext_p.h"
#include "qsgtextinput_p.h"
#include "qsgtextedit_p.h"
#include "qsgimage_p.h"
#include "qsgborderimage_p.h"
#include "qsgscalegrid_p_p.h"
#include "qsgmousearea_p.h"
#include "qsgpincharea_p.h"
#include "qsgflickable_p.h"
#include "qsgflickable_p_p.h"
#include "qsglistview_p.h"
#include "qsgvisualitemmodel_p.h"
#include "qsggridview_p.h"
#include "qsgpathview_p.h"
#include <private/qdeclarativepath_p.h>
#include "qsgpositioners_p.h"
#include "qsgrepeater_p.h"
#include "qsgloader_p.h"
#include "qsganimatedimage_p.h"
#include "qsgflipable_p.h"
#include "qsgtranslate_p.h"
#include "qsgstateoperations_p.h"
#include "qsganimation_p.h"
#include <private/qsgshadereffectitem_p.h>
#include <private/qsgshadereffectsource_p.h>
//#include "private/qsgpincharea_p.h"

static QDeclarativePrivate::AutoParentResult qsgitem_autoParent(QObject *obj, QObject *parent)
{
    QSGItem *item = qobject_cast<QSGItem *>(obj);
    if (!item)
        return QDeclarativePrivate::IncompatibleObject;

    QSGItem *parentItem = qobject_cast<QSGItem *>(parent);
    if (!parentItem)
        return QDeclarativePrivate::IncompatibleParent;

    item->setParentItem(parentItem);
    return QDeclarativePrivate::Parented;
}

static void qt_sgitems_defineModule(const char *uri, int major, int minor)
{
    QDeclarativePrivate::RegisterAutoParent autoparent = { 0, &qsgitem_autoParent };
    QDeclarativePrivate::qmlregister(QDeclarativePrivate::AutoParentRegistration, &autoparent);

#ifdef QT_NO_MOVIE
    qmlRegisterTypeNotAvailable(uri,major,minor,"AnimatedImage", qApp->translate("QSGAnimatedImage","Qt was built without support for QMovie"));
#else
    qmlRegisterType<QSGAnimatedImage>(uri,major,minor,"AnimatedImage");
#endif
    qmlRegisterType<QSGBorderImage>(uri,major,minor,"BorderImage");
    qmlRegisterType<QSGColumn>(uri,major,minor,"Column");
    qmlRegisterType<QSGDrag>(uri,major,minor,"Drag");
    qmlRegisterType<QSGFlickable>(uri,major,minor,"Flickable");
    qmlRegisterType<QSGFlipable>(uri,major,minor,"Flipable");
    qmlRegisterType<QSGFlow>(uri,major,minor,"Flow");
//    qmlRegisterType<QDeclarativeFocusPanel>(uri,major,minor,"FocusPanel");
    qmlRegisterType<QSGFocusScope>(uri,major,minor,"FocusScope");
    qmlRegisterType<QSGGradient>(uri,major,minor,"Gradient");
    qmlRegisterType<QSGGradientStop>(uri,major,minor,"GradientStop");
    qmlRegisterType<QSGGrid>(uri,major,minor,"Grid");
    qmlRegisterType<QSGGridView>(uri,major,minor,"GridView");
    qmlRegisterType<QSGImage>(uri,major,minor,"Image");
    qmlRegisterType<QSGItem>(uri,major,minor,"Item");
    qmlRegisterType<QSGListView>(uri,major,minor,"ListView");
    qmlRegisterType<QSGLoader>(uri,major,minor,"Loader");
    qmlRegisterType<QSGMouseArea>(uri,major,minor,"MouseArea");
    qmlRegisterType<QDeclarativePath>(uri,major,minor,"Path");
    qmlRegisterType<QDeclarativePathAttribute>(uri,major,minor,"PathAttribute");
    qmlRegisterType<QDeclarativePathCubic>(uri,major,minor,"PathCubic");
    qmlRegisterType<QDeclarativePathLine>(uri,major,minor,"PathLine");
    qmlRegisterType<QDeclarativePathPercent>(uri,major,minor,"PathPercent");
    qmlRegisterType<QDeclarativePathQuad>(uri,major,minor,"PathQuad");
    qmlRegisterType<QSGPathView>(uri,major,minor,"PathView");
#ifndef QT_NO_VALIDATOR
    qmlRegisterType<QIntValidator>(uri,major,minor,"IntValidator");
    qmlRegisterType<QDoubleValidator>(uri,major,minor,"DoubleValidator");
    qmlRegisterType<QRegExpValidator>(uri,major,minor,"RegExpValidator");
#endif
    qmlRegisterType<QSGRectangle>(uri,major,minor,"Rectangle");
    qmlRegisterType<QSGRepeater>(uri,major,minor,"Repeater");
    qmlRegisterType<QSGRow>(uri,major,minor,"Row");
    qmlRegisterType<QSGTranslate>(uri,major,minor,"Translate");
    qmlRegisterType<QSGRotation>(uri,major,minor,"Rotation");
    qmlRegisterType<QSGScale>(uri,major,minor,"Scale");
    qmlRegisterType<QSGText>(uri,major,minor,"Text");
    qmlRegisterType<QSGTextEdit>(uri,major,minor,"TextEdit");
    qmlRegisterType<QSGTextInput>(uri,major,minor,"TextInput");
    qmlRegisterType<QSGViewSection>(uri,major,minor,"ViewSection");
    qmlRegisterType<QSGVisualDataModel>(uri,major,minor,"VisualDataModel");
    qmlRegisterType<QSGVisualItemModel>(uri,major,minor,"VisualItemModel");

    qmlRegisterType<QSGAnchors>();
    qmlRegisterType<QSGKeyEvent>();
    qmlRegisterType<QSGMouseEvent>();
    qmlRegisterType<QSGTransform>();
    qmlRegisterType<QDeclarativePathElement>();
    qmlRegisterType<QDeclarativeCurve>();
    qmlRegisterType<QSGScaleGrid>();
#ifndef QT_NO_VALIDATOR
    qmlRegisterType<QValidator>();
#endif
    qmlRegisterType<QSGVisualModel>();
#ifndef QT_NO_ACTION
    qmlRegisterType<QAction>();
#endif
    qmlRegisterType<QSGPen>();
    qmlRegisterType<QSGFlickableVisibleArea>();
    qRegisterMetaType<QSGAnchorLine>("QSGAnchorLine");

    qmlRegisterUncreatableType<QSGKeyNavigationAttached>(uri,major,minor,"KeyNavigation",QSGKeyNavigationAttached::tr("KeyNavigation is only available via attached properties"));
    qmlRegisterUncreatableType<QSGKeysAttached>(uri,major,minor,"Keys",QSGKeysAttached::tr("Keys is only available via attached properties"));
    qmlRegisterUncreatableType<QSGLayoutMirroringAttached>(uri,major,minor,"LayoutMirroring", QSGLayoutMirroringAttached::tr("LayoutMirroring is only available via attached properties"));

    qmlRegisterType<QSGPinchArea>(uri,major,minor,"PinchArea");
    qmlRegisterType<QSGPinch>(uri,major,minor,"Pinch");
    qmlRegisterType<QSGPinchEvent>();

    qmlRegisterType<QSGShaderEffectItem>("QtQuick", 2, 0, "ShaderEffectItem");
    qmlRegisterType<QSGShaderEffectSource>("QtQuick", 2, 0, "ShaderEffectSource");
    qmlRegisterUncreatableType<QSGShaderEffectMesh>("QtQuick", 2, 0, "ShaderEffectMesh", QSGShaderEffectMesh::tr("Cannot create instance of abstract class ShaderEffectMesh."));
    qmlRegisterType<QSGGridMesh>("QtQuick", 2, 0, "GridMesh");

    qmlRegisterUncreatableType<QSGPaintedItem>("QtQuick", 2, 0, "PaintedItem", QSGPaintedItem::tr("Cannot create instance of abstract class PaintedItem"));

    qmlRegisterType<QSGParentChange>(uri, major, minor,"ParentChange");
    qmlRegisterType<QSGAnchorChanges>(uri, major, minor,"AnchorChanges");
    qmlRegisterType<QSGAnchorSet>();
    qmlRegisterType<QSGAnchorAnimation>(uri, major, minor,"AnchorAnimation");
    qmlRegisterType<QSGParentAnimation>(uri, major, minor,"ParentAnimation");
}

void QSGItemsModule::defineModule()
{
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;

    // XXX todo -  Remove before final integration...
    QByteArray mode = qgetenv("QMLSCENE_IMPORT_NAME");
    QByteArray name = "QtQuick";
    int majorVersion = 2;
    int minorVersion = 0;
    if (mode == "quick1") {
        majorVersion = 1;
    } else if (mode == "qt") {
        name = "Qt";
        majorVersion = 4;
        minorVersion = 7;
    }

    qt_sgitems_defineModule(name, majorVersion, minorVersion);
}

