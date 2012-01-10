/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
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
import Qt.labs.shaders 1.0

Rectangle {
    id: marginTester
    anchors.fill: parent;
    color: "green"
    property real testMarginX: 10
    property real testMarginY: 10

    Timer {
        running: true
        interval: 2000
        repeat: true
        onTriggered: {
            if (marginTester.testMarginX < 20) {
                marginTester.testMarginX = 50
                marginTester.testMarginY = 120
            }
            else {
                marginTester.testMarginX = 10
                marginTester.testMarginY = 10
            }

            theSource.sourceRect = Qt.rect(-testMarginX, -testMarginY, parent.width + testMarginX*2, parent.height + testMarginY*2)
        }
    }

    Image {
        id: redrect
        source: "image_opaque.png"
    }

    ShaderEffectSource {
        id: theSource
        sourceItem: redrect
        textureSize: Qt.size(160,160)
        sourceRect: Qt.rect(-10,-10, parent.width + 2*10, parent.height + 2*10)
        hideSource: true
    }

    ShaderEffectItem {
        id: effect
        anchors.fill: parent;
        property variant source: theSource
    }

    Rectangle {
        width: parent.width
        height: 40
        color: "#cc000000"

        Text {
             id: label
             anchors.centerIn:  parent
             text: theSource.sourceRect.width == 0 ? "No margins" : "Green margins " + marginTester.testMarginX + "x" + marginTester.testMarginY
             color: "white"
             font.bold: true
        }
    }
}
