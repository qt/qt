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

#include "qmlgraphicsutilmodule_p.h"
#include "qfxperf_p_p.h"
#include "qmlanimation_p.h"
#include "qmlanimation_p_p.h"
#include "qmlbehavior_p.h"
#include "qmlbind_p.h"
#include "qmlconnection_p.h"
#include "qmldatetimeformatter_p.h"
#include "qmleasefollow_p.h"
#include "qmlfontloader_p.h"
#include "qmllistaccessor_p.h"
#include "qmllistmodel_p.h"
#include "qmlnullablevalue_p_p.h"
#include "qmlnumberformatter_p.h"
#include "qmlopenmetaobject_p.h"
#include "qmlpackage_p.h"
#include "qmlpixmapcache_p.h"
#include "qmlpropertychanges_p.h"
#include "qmlpropertymap.h"
#include "qmlspringfollow_p.h"
#include "qmlstategroup_p.h"
#include "qmlstateoperations_p.h"
#include "qmlstate_p.h"
#include "qmlstate_p_p.h"
#include "qmlstyledtext_p.h"
#include "qmlsystempalette_p.h"
#include "qmltimeline_p_p.h"
#include "qmltimer_p.h"
#include "qmltransitionmanager_p_p.h"
#include "qmltransition_p.h"
#include "qmlview.h"
#include "qmlxmllistmodel_p.h"
#include "qnumberformat_p.h"
#include "qperformancelog_p_p.h"

void QmlGraphicsUtilModule::defineModule()
{
    QML_REGISTER_TYPE(Qt,4,6,AnchorChanges,QmlAnchorChanges);
    QML_REGISTER_TYPE(Qt,4,6,Behavior,QmlBehavior);
    QML_REGISTER_TYPE(Qt,4,6,Binding,QmlBind);
    QML_REGISTER_TYPE(Qt,4,6,ColorAnimation,QmlColorAnimation);
    QML_REGISTER_TYPE(Qt,4,6,Connection,QmlConnection);
    QML_REGISTER_TYPE(Qt,4,6,DateTimeFormatter,QmlDateTimeFormatter);
    QML_REGISTER_TYPE(Qt,4,6,EaseFollow,QmlEaseFollow);;
    QML_REGISTER_TYPE(Qt,4,6,FontLoader,QmlFontLoader);
    QML_REGISTER_TYPE(Qt,4,6,ListElement,QmlListElement);
    QML_REGISTER_TYPE(Qt,4,6,NumberAnimation,QmlNumberAnimation);
    QML_REGISTER_TYPE(Qt,4,6,NumberFormatter,QmlNumberFormatter);;
    QML_REGISTER_TYPE(Qt,4,6,Package,QmlPackage);
    QML_REGISTER_TYPE(Qt,4,6,ParallelAnimation,QmlParallelAnimation);
    QML_REGISTER_TYPE(Qt,4,6,ParentAction,QmlParentAction);
    QML_REGISTER_TYPE(Qt,4,6,ParentChange,QmlParentChange);
    QML_REGISTER_TYPE(Qt,4,6,PauseAnimation,QmlPauseAnimation);
    QML_REGISTER_TYPE(Qt,4,6,PropertyAction,QmlPropertyAction);
    QML_REGISTER_TYPE(Qt,4,6,PropertyAnimation,QmlPropertyAnimation);
    QML_REGISTER_TYPE(Qt,4,6,RotationAnimation,QmlRotationAnimation);
    QML_REGISTER_TYPE(Qt,4,6,ScriptAction,QmlScriptAction);
    QML_REGISTER_TYPE(Qt,4,6,SequentialAnimation,QmlSequentialAnimation);
    QML_REGISTER_TYPE(Qt,4,6,SpringFollow,QmlSpringFollow);
    QML_REGISTER_TYPE(Qt,4,6,StateChangeScript,QmlStateChangeScript);
    QML_REGISTER_TYPE(Qt,4,6,StateGroup,QmlStateGroup);
    QML_REGISTER_TYPE(Qt,4,6,State,QmlState);
    QML_REGISTER_TYPE(Qt,4,6,SystemPalette,QmlSystemPalette);
    QML_REGISTER_TYPE(Qt,4,6,Timer,QmlTimer);
    QML_REGISTER_TYPE(Qt,4,6,Transition,QmlTransition);
    QML_REGISTER_TYPE(Qt,4,6,Vector3dAnimation,QmlVector3dAnimation);
    QML_REGISTER_TYPE(Qt,4,6,XmlListModel,QmlXmlListModel);
    QML_REGISTER_TYPE(Qt,4,6,XmlRole,QmlXmlListModelRole);

    QML_REGISTER_NOCREATE_TYPE(QmlGraphicsAnchors);
    QML_REGISTER_NOCREATE_TYPE(QmlAbstractAnimation);
    QML_REGISTER_NOCREATE_TYPE(QmlStateOperation);
    QML_REGISTER_NOCREATE_TYPE(QNumberFormat);

    QML_REGISTER_CUSTOM_TYPE(Qt, 4,6, ListModel, QmlListModel, QmlListModelParser);
    QML_REGISTER_CUSTOM_TYPE(Qt, 4,6, PropertyChanges, QmlPropertyChanges, QmlPropertyChangesParser);
}
