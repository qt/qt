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

Rectangle {
    width: 180; height: 200; color: "white"

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
            width: 180; height: 40
            Column {
                x: 5; y: 5
                Text { text: '<b>Name:</b> ' + name }
                Text { text: '<b>Number:</b> ' + number }
            }
            // Use the ListView.isCurrentItem attached property to
            // indent the item if it is the current item.
            states: [
                State {
                    name: "Current"
                    when: wrapper.ListView.isCurrentItem
                    PropertyChanges { target: wrapper; x: 10 }
                }
            ]
            transitions: [
                Transition { NumberAnimation { properties: "x"; duration: 200 } }
            ]
        }
    }
//! [0]
    // Specify a highlight with custom movement.  Note that autoHighlight
    // is set to false in the ListView so that we can control how the
    // highlight moves to the current item.
//! [1]
    Component {
        id: highlight
        Rectangle {
            width: 180; height: 40
            color: "lightsteelblue"; radius: 5
            SpringFollow on y {
                to: list.currentItem.y
                spring: 3
                damping: 0.2
            }
        }
    }
    ListView {
        id: list
        width: parent.height; height: parent.height
        model: ContactModel; delegate: delegate
        highlight: highlight
        highlightFollowsCurrentItem: false
        focus: true
    }
//! [1]
}
