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

import QtQuick 1.0

Item { id: wrapper
    property variant model: xmlModel
    property string from : ""
    property string to : ""
    property string phrase : ""

    property string mode : "everyone"
    property int status: xmlModel.status
    function reload() { xmlModel.reload(); }
    XmlListModel {
        id: xmlModel

        source: (from=="" && to=="" && phrase=="") ? "" :
            'http://search.twitter.com/search.atom?from='+from+"&to="+to+"&phrase="+phrase

        namespaceDeclarations: "declare default element namespace 'http://www.w3.org/2005/Atom'; " +
                               "declare namespace twitter=\"http://api.twitter.com/\";";

        query: "/feed/entry"

        XmlRole { name: "statusText"; query: "content/string()" }
        XmlRole { name: "timestamp"; query: "published/string()" }
        XmlRole { name: "source"; query: "twitter:source/string()" }
        XmlRole { name: "name"; query: "author/name/string()" }
        XmlRole { name: "userImage"; query: "link[@rel = 'image']/@href/string()" }

    }
    Binding {
        property: "mode"
        target: wrapper
        value: {if(wrapper.tags==''){"everyone";}else if(wrapper.tags=='my timeline'){"self";}else{"user";}}
    }
}
