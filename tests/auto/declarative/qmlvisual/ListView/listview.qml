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
    width: 600; height: 300; color: "white"

    ListModel {
        id: myModel
        ListElement {
            itemColor: "red"
        }
        ListElement {
            itemColor: "green"
        }
        ListElement {
            itemColor: "blue"
        }
        ListElement {
            itemColor: "orange"
        }
        ListElement {
            itemColor: "brown"
        }
        ListElement {
            itemColor: "yellow"
        }
        ListElement {
            itemColor: "purple"
        }
        ListElement {
            itemColor: "darkred"
        }
        ListElement {
            itemColor: "darkblue"
        }
    }

    Component {
        id: myDelegate
        Item {
            width: 200; height: 50
            Rectangle {
                x: 5; y : 5
                width: 190; height: 40
                opacity: 0.5
                color: itemColor
            }
        }
    }

    Component {
        id: myHighlight
        Rectangle { color: "black" }
    }

    ListView {
        id: list1
        width: 200; height: parent.height
        model: myModel; delegate: myDelegate
        highlight: myHighlight; currentIndex: list3.currentIndex
        focus: true
    }
    ListView {
        id: list2
        x: 200; width: 200; height: parent.height
        model: myModel; delegate: myDelegate; highlight: myHighlight
        preferredHighlightBegin: 80
        preferredHighlightEnd: 220
        highlightRangeMode: "ApplyRange"
        currentIndex: list1.currentIndex
    }
    ListView {
        id: list3
        x: 400; width: 200; height: parent.height
        model: myModel; delegate: myDelegate; highlight: myHighlight
        currentIndex: list1.currentIndex
        preferredHighlightBegin: 125
        preferredHighlightEnd: 125
        highlightRangeMode: "StrictlyEnforceRange"
        flickDeceleration: 1000
    }
}
