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

//! [3]
Rectangle {
    width: 240; height: 180; color: "white"
    // ContactModel model is defined in dummydata/ContactModel.qml
    // The viewer automatically loads files in dummydata/* to assist
    // development without a real data source.

    // Define a delegate component.  A component will be
    // instantiated for each visible item in the list.
//! [0]
    Component {
        id: delegate
        Item {
            id: wrapper
            width: 80; height: 78
            Column {
                Image { source: portrait; anchors.horizontalCenter: parent.horizontalCenter }
                Text { text: name; anchors.horizontalCenter: parent.horizontalCenter }
            }
        }
    }
//! [0]
    // Define a highlight component.  Just one of these will be instantiated
    // by each ListView and placed behind the current item.
//! [1]
    Component {
        id: highlight
        Rectangle {
            color: "lightsteelblue"
            radius: 5
        }
    }
//! [1]
    // The actual grid
//! [2]
    GridView {
        width: parent.width; height: parent.height
        model: ContactModel; delegate: delegate
        cellWidth: 80; cellHeight: 80
        highlight: highlight
        focus: true
    }
//! [2]
}
//! [3]
