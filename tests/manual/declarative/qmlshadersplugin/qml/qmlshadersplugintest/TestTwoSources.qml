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
    color: "black"

    Rectangle {
        id: rect;
        anchors.centerIn: parent
        width: 1
        height: 10

        gradient: Gradient {
            GradientStop { position: 0; color: "#ff0000" }
            GradientStop { position: 0.5; color: "#00ff00" }
            GradientStop { position: 1; color: "#0000ff" }
        }
    }

    Text {
        id: text
        anchors.centerIn: parent
        font.pixelSize:  80
        text: "Shaderz!"
    }

    ShaderEffectSource {
        id: maskSource
        sourceItem: text
        hideSource: true
    }

    ShaderEffectSource {
        id: colorSource
        sourceItem: rect;
        hideSource: true
    }

    ShaderEffectItem {
        anchors.fill: text;

        property variant colorSource: colorSource
        property variant maskSource: maskSource;

        fragmentShader: "
        uniform lowp sampler2D maskSource;
        uniform lowp sampler2D colorSource;
        varying highp vec2 qt_TexCoord0;
        void main() {
            gl_FragColor = texture2D(maskSource, qt_TexCoord0).a * texture2D(colorSource, qt_TexCoord0.yx);
        }
        "
    }
}
