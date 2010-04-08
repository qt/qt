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

#include "private/qdeclarativeutilmodule_p.h"
#include "private/qdeclarativeanimation_p.h"
#include "private/qdeclarativeanimation_p_p.h"
#include "private/qdeclarativebehavior_p.h"
#include "private/qdeclarativebind_p.h"
#include "private/qdeclarativeconnections_p.h"
#include "private/qdeclarativesmoothedanimation_p.h"
#include "private/qdeclarativesmoothedfollow_p.h"
#include "private/qdeclarativefontloader_p.h"
#include "private/qdeclarativelistaccessor_p.h"
#include "private/qdeclarativelistmodel_p.h"
#include "private/qdeclarativenullablevalue_p_p.h"
#include "private/qdeclarativeopenmetaobject_p.h"
#include "private/qdeclarativepackage_p.h"
#include "private/qdeclarativepixmapcache_p.h"
#include "private/qdeclarativepropertychanges_p.h"
#include "qdeclarativepropertymap.h"
#include "private/qdeclarativespringfollow_p.h"
#include "private/qdeclarativestategroup_p.h"
#include "private/qdeclarativestateoperations_p.h"
#include "private/qdeclarativestate_p.h"
#include "private/qdeclarativestate_p_p.h"
#include "private/qdeclarativestyledtext_p.h"
#include "private/qdeclarativesystempalette_p.h"
#include "private/qdeclarativetimeline_p_p.h"
#include "private/qdeclarativetimer_p.h"
#include "private/qdeclarativetransitionmanager_p_p.h"
#include "private/qdeclarativetransition_p.h"
#include "qdeclarativeview.h"
#ifndef QT_NO_XMLPATTERNS
#include "private/qdeclarativexmllistmodel_p.h"
#endif

void QDeclarativeUtilModule::defineModule()
{
    qmlRegisterType<QDeclarativeAnchorAnimation>("Qt",4,6,"AnchorAnimation");
    qmlRegisterType<QDeclarativeAnchorChanges>("Qt",4,6,"AnchorChanges");
    qmlRegisterType<QDeclarativeBehavior>("Qt",4,6,"Behavior");
    qmlRegisterType<QDeclarativeBind>("Qt",4,6,"Binding");
    qmlRegisterType<QDeclarativeColorAnimation>("Qt",4,6,"ColorAnimation");
    qmlRegisterType<QDeclarativeConnections>("Qt",4,6,"Connections");
    qmlRegisterType<QDeclarativeSmoothedAnimation>("Qt",4,6,"SmoothedAnimation");
    qmlRegisterType<QDeclarativeSmoothedFollow>("Qt",4,6,"SmoothedFollow");
    qmlRegisterType<QDeclarativeFontLoader>("Qt",4,6,"FontLoader");
    qmlRegisterType<QDeclarativeListElement>("Qt",4,6,"ListElement");
    qmlRegisterType<QDeclarativeNumberAnimation>("Qt",4,6,"NumberAnimation");
    qmlRegisterType<QDeclarativePackage>("Qt",4,6,"Package");
    qmlRegisterType<QDeclarativeParallelAnimation>("Qt",4,6,"ParallelAnimation");
    qmlRegisterType<QDeclarativeParentAnimation>("Qt",4,6,"ParentAnimation");
    qmlRegisterType<QDeclarativeParentChange>("Qt",4,6,"ParentChange");
    qmlRegisterType<QDeclarativePauseAnimation>("Qt",4,6,"PauseAnimation");
    qmlRegisterType<QDeclarativePropertyAction>("Qt",4,6,"PropertyAction");
    qmlRegisterType<QDeclarativePropertyAnimation>("Qt",4,6,"PropertyAnimation");
    qmlRegisterType<QDeclarativeRotationAnimation>("Qt",4,6,"RotationAnimation");
    qmlRegisterType<QDeclarativeScriptAction>("Qt",4,6,"ScriptAction");
    qmlRegisterType<QDeclarativeSequentialAnimation>("Qt",4,6,"SequentialAnimation");
    qmlRegisterType<QDeclarativeSpringFollow>("Qt",4,6,"SpringFollow");
    qmlRegisterType<QDeclarativeStateChangeScript>("Qt",4,6,"StateChangeScript");
    qmlRegisterType<QDeclarativeStateGroup>("Qt",4,6,"StateGroup");
    qmlRegisterType<QDeclarativeState>("Qt",4,6,"State");
    qmlRegisterType<QDeclarativeSystemPalette>("Qt",4,6,"SystemPalette");
    qmlRegisterType<QDeclarativeTimer>("Qt",4,6,"Timer");
    qmlRegisterType<QDeclarativeTransition>("Qt",4,6,"Transition");
    qmlRegisterType<QDeclarativeVector3dAnimation>("Qt",4,6,"Vector3dAnimation");
#ifndef QT_NO_XMLPATTERNS
    qmlRegisterType<QDeclarativeXmlListModel>("Qt",4,6,"XmlListModel");
    qmlRegisterType<QDeclarativeXmlListModelRole>("Qt",4,6,"XmlRole");
#endif

    qmlRegisterType<QDeclarativeAnchors>();
    qmlRegisterType<QDeclarativeStateOperation>();
    qmlRegisterType<QDeclarativeAnchorSet>();

    qmlRegisterUncreatableType<QDeclarativeAbstractAnimation>("Qt",4,6,"Animation");

    qmlRegisterCustomType<QDeclarativeListModel>("Qt", 4,6, "ListModel", "QDeclarativeListModel",
                                                 new QDeclarativeListModelParser);
    qmlRegisterCustomType<QDeclarativePropertyChanges>("Qt", 4, 6, "PropertyChanges", "QDeclarativePropertyChanges",
                                                       new QDeclarativePropertyChangesParser);
    qmlRegisterCustomType<QDeclarativeConnections>("Qt", 4, 6, "Connections", "QDeclarativeConnections",
                                                   new QDeclarativeConnectionsParser);
}
