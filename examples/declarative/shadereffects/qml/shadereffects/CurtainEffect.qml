/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

ShaderEffectItem {
    anchors.fill: parent
    property variant source
    meshResolution: Qt.size(50, 50)

    property real topWidth: width / 2
    property real bottomWidth: width / 2
    property real originalWidth: width
    property real originalHeight: height
    property real amplitude: 0.1

    vertexShader: "
        attribute highp vec4 qt_Vertex;
        attribute highp vec2 qt_MultiTexCoord0;
        uniform highp mat4 qt_ModelViewProjectionMatrix;
        varying highp vec2 qt_TexCoord0;
        varying lowp float shade;

        uniform highp float topWidth;
        uniform highp float bottomWidth;
        uniform highp float originalWidth;
        uniform highp float originalHeight;
        uniform highp float amplitude;

        void main() {
            qt_TexCoord0 = qt_MultiTexCoord0;

            highp vec4 shift = vec4(0, 0, 0, 0);
            shift.x = qt_Vertex.x * ((originalWidth - topWidth) + (topWidth - bottomWidth) * (qt_Vertex.y / originalHeight)) / originalWidth;

            shade = sin(21.9911486 * qt_Vertex.x / originalWidth);
            shift.y = amplitude * (originalWidth - topWidth + (topWidth - bottomWidth) * (qt_Vertex.y / originalHeight)) * shade;

            gl_Position = qt_ModelViewProjectionMatrix * (qt_Vertex - shift);

            shade = 0.2 * (2.0 - shade ) * (1.0 - (bottomWidth + (topWidth  - bottomWidth) * (1.0 - qt_Vertex.y / originalHeight)) / originalWidth);
        }
    "

    fragmentShader: "
        uniform sampler2D source;
        varying highp vec2 qt_TexCoord0;
        varying lowp float shade;
        void main() {
            highp vec4 color = texture2D(source, qt_TexCoord0);
            color.rgb *= 1.0 - shade;
            gl_FragColor = color;
        }
    "
}



