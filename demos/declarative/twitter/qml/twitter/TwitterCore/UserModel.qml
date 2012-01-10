/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0

//This "model" gets the user information about the searched user. Mainly for the icon.

Item { id: wrapper
    property variant model: xmlModel
    property string user : ""
    property int status: xmlModel.status
    function reload() { xmlModel.reload(); }
    XmlListModel {
        id: xmlModel

        source: user!= "" ? "http://twitter.com/users/show.xml?screen_name="+user : ""
        query: "/user"

        XmlRole { name: "name"; query: "name/string()" }
        XmlRole { name: "screenName"; query: "screen_name/string()" }
        XmlRole { name: "image"; query: "profile_image_url/string()" }
        XmlRole { name: "location"; query: "location/string()" }
        XmlRole { name: "description"; query: "description/string()" }
        XmlRole { name: "followers"; query: "followers_count/string()" }
        //TODO: Could also get the user's color scheme, timezone and a few other things
    }
}
