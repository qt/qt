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
    anchors.fill: parent;
    color: "white"

    Timer {
        running: true
        interval: 2000
        repeat: true
        onTriggered: {
            theSource.wrapMode == ShaderEffectSource.RepeatVertically ? theSource.wrapMode = ShaderEffectSource.ClampToEdge : theSource.wrapMode = ShaderEffectSource.RepeatVertically
        }
    }

    ShaderEffectSource {
        id: theSource
        sourceItem: Image { source: "image_small.png" }
        live: false
        hideSource: true
    }

    ShaderEffectItem {
        id: effect
        anchors.fill: parent;
        property variant source: theSource
        fragmentShader: "
        uniform lowp sampler2D source;
        varying highp vec2 qt_TexCoord0;
        void main() {
            highp vec2 tex = qt_TexCoord0 * 4.0;
            gl_FragColor = texture2D(source, tex);
        }
        "

    }

    Rectangle {
        width: parent.width
        height: 40
        color: "#cc000000"

        Text {
             id: label
             anchors.centerIn:  parent
             text: theSource.wrapMode == ShaderEffectSource.RepeatVertically ? "Wrap RepeatVertically" : "VerticalWrap ClampToEdge"
             color: "white"
             font.bold: true
        }
    }
}
