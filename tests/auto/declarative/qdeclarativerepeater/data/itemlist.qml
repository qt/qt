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

// This example demonstrates placing items in a view using
// a VisualItemModel

import Qt 4.7

Rectangle {
    color: "lightgray"
    width: 240
    height: 320

    function checkProperties() {
        testObject.error = false;
        if (testObject.useModel && view.model != itemModel) {
            console.log("model property incorrect");
            testObject.error = true;
        }
    }

    VisualItemModel {
        id: itemModel
        objectName: "itemModel"
        Rectangle {
            objectName: "item1"
            height: 50; width: 100; color: "#FFFEF0"
            Text { objectName: "text1"; text: "index: " + parent.VisualItemModel.index; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            objectName: "item2"
            height: 50; width: 100; color: "#F0FFF7"
            Text { objectName: "text2"; text: "index: " + parent.VisualItemModel.index; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            objectName: "item3"
            height: 50; width: 100; color: "#F4F0FF"
            Text { objectName: "text3"; text: "index: " + parent.VisualItemModel.index; font.bold: true; anchors.centerIn: parent }
        }
    }

    Column {
        objectName: "container"
        Repeater {
            id: view
            objectName: "repeater"
            model: testObject.useModel ? itemModel : 0
        }
    }
}
