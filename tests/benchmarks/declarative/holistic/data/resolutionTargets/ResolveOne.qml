/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

// This file has some nested items and does a lot of id resolution.
// This will allow us to benchmark the cost of resolving names in
// bindings.

import QtQuick 1.0

Item {
    id: root
    property int baseWidth: 500
    property int baseHeight: 600
    property string baseColor: "red"

    Item {
        id: childOne
        property int baseWidth: root.baseWidth - 1
        property int baseHeight: root.baseHeight - 1
        property string baseColor: root.baseColor

        Item {
            id: childTwo
            property int baseWidth: root.baseWidth - 2
            property int baseHeight: childOne.baseHeight - 1
            property string baseColor: childOne.baseColor

            Item {
                id: childThree
                property int baseWidth: childOne.baseWidth - 2
                property int baseHeight: root.baseHeight - 3
                property string baseColor: "blue"

                Item {
                    id: childFour
                    property int baseWidth: childTwo.baseWidth - 2
                    property int baseHeight: childThree.baseHeight - 1
                    property string baseColor: "earthy " + root.baseColor

                    Item {
                        id: childFive
                        property int baseWidth: root.baseWidth - 5
                        property int baseHeight: childFour.baseHeight - 1
                        property string baseColor: "carnelian " + childTwo.baseColor
                    }
                }

                Item {
                    id: childSix
                    property int baseWidth: parent.baseWidth - 3
                    property int baseHeight: root.baseHeight - 6
                    property string baseColor: "rust " + root.baseColor

                    Item {
                        id: childSeven
                        property int baseWidth: childOne.baseWidth - 6
                        property int baseHeight: childTwo.baseHeight - 5
                        property string baseColor: "sky " + childThree.baseColor
                    }
                }
            }
        }
    }

    Rectangle {
        width: childOne.baseWidth
        height: childOne.baseHeight
        color: parent.baseColor
        border.color: "black"
        border.width: 5
        radius: 10
    }
}
