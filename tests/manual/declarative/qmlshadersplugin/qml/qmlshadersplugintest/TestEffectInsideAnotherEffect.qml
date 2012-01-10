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
    color: "green"

    Timer {
        running: true
        interval: 2000
        repeat: true
        onTriggered: {

        }
    }

    Rectangle {
        id: theSource
        color: "red"
        anchors.centerIn: parent;
        width: parent.width/2
        height: parent.height/2
    }

    ShaderEffectItem {
        id: effect1
        anchors.fill: theSource;
        property variant source: ShaderEffectSource{ sourceItem: theSource; hideSource: true }
    }

    ShaderEffectItem {
        id: effect2
        anchors.fill: effect1;
        property variant source: effect1

        fragmentShader: "
            varying highp vec2 qt_TexCoord0;
            uniform sampler2D source;
            void main(void)
            {
                gl_FragColor = vec4(texture2D(source, qt_TexCoord0.st).rgb, 1.0);
            }
        "
    }

    ShaderEffectItem {
         id: effect3
         x: effect2.x
         y: effect2.y
         width: effect2.width
         height: effect2.height

         property variant source: ShaderEffectSource { sourceItem: effect2 ; hideSource: false }

         fragmentShader:
             "
              varying highp vec2 qt_TexCoord0;
              uniform sampler2D source;
              void main(void)
                  {
                      gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
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
             text: "Red rect inside green fullscreen rect."
             color: "white"
             font.bold: true
        }
    }
}
