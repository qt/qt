/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
import Qt 4.7

Rectangle {
    width: 200; height: 200


//![model]
ListModel {
    id: fruitModel

    ListElement {
        name: "Apple"
        cost: 2.45
        attributes: [
            ListElement { description: "Core" },
            ListElement { description: "Deciduous" }
        ]
    }
    ListElement {
        name: "Orange"
        cost: 3.25
        attributes: [
            ListElement { description: "Citrus" }
        ]
    }
    ListElement {
        name: "Banana"
        cost: 1.95
        attributes: [
            ListElement { description: "Tropical" },
            ListElement { description: "Seedless" }
        ]
    }
}
//![model]

//![delegate]
Component {
    id: fruitDelegate
    Item {
        width: 200; height: 50
        Text { id: nameField; text: name }
        Text { text: '$' + cost; anchors.left: nameField.right }
        Row {
            anchors.top: nameField.bottom
            spacing: 5
            Text { text: "Attributes:" }
            Repeater {
                model: attributes
                Text { text: description } 
            }
        }
    }
}
//![delegate]

ListView {
    width: 200; height: 200
    model: fruitModel
    delegate: fruitDelegate
}

}
