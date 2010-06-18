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

import Qt 4.7

//This "model" gets the user information about the searched user. Mainly for the icon.
//Copied from RssModel

Item { id: wrapper
    property variant model: xmlModel
    property string user : ""
    property int status: xmlModel.status
    function reload() { xmlModel.reload(); }
XmlListModel {
    id: xmlModel

    source: {if(user!="") {"http://twitter.com/users/show.xml?screen_name="+user;}else{"";}}
    query: "/user"

    XmlRole { name: "name"; query: "name/string()" }
    XmlRole { name: "screenName"; query: "screen_name/string()" }
    XmlRole { name: "image"; query: "profile_image_url/string()" }
    XmlRole { name: "location"; query: "location/string()" }
    XmlRole { name: "description"; query: "description/string()" }
    XmlRole { name: "followers"; query: "followers_count/string()" }
    //XmlRole { name: "protected"; query: "protected/bool()" }
    //TODO: Could also get the user's color scheme, timezone and a few other things
}
}
