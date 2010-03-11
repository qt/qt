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

#include "qdeclarativeutilmodule_p.h"
#include "qfxperf_p_p.h"
#include "qdeclarativeanimation_p.h"
#include "qdeclarativeanimation_p_p.h"
#include "qdeclarativebehavior_p.h"
#include "qdeclarativebind_p.h"
#include "qdeclarativeconnections_p.h"
#include "qdeclarativeeasefollow_p.h"
#include "qdeclarativefontloader_p.h"
#include "qdeclarativelistaccessor_p.h"
#include "qdeclarativelistmodel_p.h"
#include "qdeclarativenullablevalue_p_p.h"
#include "qdeclarativeopenmetaobject_p.h"
#include "qdeclarativepackage_p.h"
#include "qdeclarativepixmapcache_p.h"
#include "qdeclarativepropertychanges_p.h"
#include "qdeclarativepropertymap.h"
#include "qdeclarativespringfollow_p.h"
#include "qdeclarativestategroup_p.h"
#include "qdeclarativestateoperations_p.h"
#include "qdeclarativestate_p.h"
#include "qdeclarativestate_p_p.h"
#include "qdeclarativestyledtext_p.h"
#include "qdeclarativesystempalette_p.h"
#include "qdeclarativetimeline_p_p.h"
#include "qdeclarativetimer_p.h"
#include "qdeclarativetransitionmanager_p_p.h"
#include "qdeclarativetransition_p.h"
#include "qdeclarativeview.h"
#ifndef QT_NO_XMLPATTERNS
#include "qdeclarativexmllistmodel_p.h"
#endif
#include "qperformancelog_p_p.h"

void QDeclarativeUtilModule::defineModule()
{
    QML_REGISTER_TYPE(Qt,4,6,AnchorChanges,QDeclarativeAnchorChanges);
    QML_REGISTER_TYPE(Qt,4,6,Behavior,QDeclarativeBehavior);
    QML_REGISTER_TYPE(Qt,4,6,Binding,QDeclarativeBind);
    QML_REGISTER_TYPE(Qt,4,6,ColorAnimation,QDeclarativeColorAnimation);
    QML_REGISTER_TYPE(Qt,4,6,Connections,QDeclarativeConnections);
    QML_REGISTER_TYPE(Qt,4,6,EaseFollow,QDeclarativeEaseFollow);;
    QML_REGISTER_TYPE(Qt,4,6,FontLoader,QDeclarativeFontLoader);
    QML_REGISTER_TYPE(Qt,4,6,ListElement,QDeclarativeListElement);
    QML_REGISTER_TYPE(Qt,4,6,NumberAnimation,QDeclarativeNumberAnimation);
    QML_REGISTER_TYPE(Qt,4,6,Package,QDeclarativePackage);
    QML_REGISTER_TYPE(Qt,4,6,ParallelAnimation,QDeclarativeParallelAnimation);
    QML_REGISTER_TYPE(Qt,4,6,ParentAction,QDeclarativeParentAction);
    QML_REGISTER_TYPE(Qt,4,6,ParentAnimation,QDeclarativeParentAnimation);
    QML_REGISTER_TYPE(Qt,4,6,ParentChange,QDeclarativeParentChange);
    QML_REGISTER_TYPE(Qt,4,6,PauseAnimation,QDeclarativePauseAnimation);
    QML_REGISTER_TYPE(Qt,4,6,PropertyAction,QDeclarativePropertyAction);
    QML_REGISTER_TYPE(Qt,4,6,PropertyAnimation,QDeclarativePropertyAnimation);
    QML_REGISTER_TYPE(Qt,4,6,RotationAnimation,QDeclarativeRotationAnimation);
    QML_REGISTER_TYPE(Qt,4,6,ScriptAction,QDeclarativeScriptAction);
    QML_REGISTER_TYPE(Qt,4,6,SequentialAnimation,QDeclarativeSequentialAnimation);
    QML_REGISTER_TYPE(Qt,4,6,SpringFollow,QDeclarativeSpringFollow);
    QML_REGISTER_TYPE(Qt,4,6,StateChangeScript,QDeclarativeStateChangeScript);
    QML_REGISTER_TYPE(Qt,4,6,StateGroup,QDeclarativeStateGroup);
    QML_REGISTER_TYPE(Qt,4,6,State,QDeclarativeState);
    QML_REGISTER_TYPE(Qt,4,6,SystemPalette,QDeclarativeSystemPalette);
    QML_REGISTER_TYPE(Qt,4,6,Timer,QDeclarativeTimer);
    QML_REGISTER_TYPE(Qt,4,6,Transition,QDeclarativeTransition);
    QML_REGISTER_TYPE(Qt,4,6,Vector3dAnimation,QDeclarativeVector3dAnimation);
#ifndef QT_NO_XMLPATTERNS
    QML_REGISTER_TYPE(Qt,4,6,XmlListModel,QDeclarativeXmlListModel);
    QML_REGISTER_TYPE(Qt,4,6,XmlRole,QDeclarativeXmlListModelRole);
#endif

    QML_REGISTER_NOCREATE_TYPE(QDeclarativeAnchors);
    QML_REGISTER_NOCREATE_TYPE(QDeclarativeAbstractAnimation);
    QML_REGISTER_NOCREATE_TYPE(QDeclarativeStateOperation);

    QML_REGISTER_CUSTOM_TYPE(Qt, 4,6, ListModel, QDeclarativeListModel, QDeclarativeListModelParser);
    QML_REGISTER_CUSTOM_TYPE(Qt, 4,6, PropertyChanges, QDeclarativePropertyChanges, QDeclarativePropertyChangesParser);
    QML_REGISTER_CUSTOM_TYPE(Qt, 4,6, Connections, QDeclarativeConnections, QDeclarativeConnectionsParser);
}
