/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
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

import QtQuick 2.0

Image {
    width: 640
    height: 360
    source: "../snake/content/pics/background.png"

    ShaderEffectSource {
        id: theSource
        sourceItem: theItem
        smooth: true
    }

    function saturate(x) {
        return Math.min(Math.max(x, 0), 1)
    }

    function sliderToColor(x) {
        return Qt.rgba(saturate(Math.max(2 - 6 * x, 6 * x - 4)),
                        saturate(Math.min(6 * x, 4 - 6 * x)),
                        saturate(Math.min(6 * x - 2, 6 - 6 * x)))
    }

    Grid {
        anchors.centerIn: parent
        columns: 3

        Item {
            id: theItem
            width: 180
            height: 180
            ListView {
                anchors.centerIn: parent
                width: 160
                height: 140
                clip: true
                snapMode: ListView.SnapOneItem
                model: VisualItemModel {
                    Text {
                        width: 160
                        height: 140
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 120
                        font.family: "Times"
                        color: "blue"
                        text: "Qt"
                    }
                    Image {
                        width: 160
                        height: 140
                        source: "qt-logo.png"
                        smooth: true
                    }
                    Image {
                        width: 160
                        height: 140
                        source: "face-smile.png"
                        smooth: true
                    }
                }
            }
        }
        ShaderEffectItem {
            width: 180
            height: 180
            property variant source: theSource
            property real amplitude: 0.04 * wobbleSlider.value
            property real frequency: 20
            property real time: 0
            NumberAnimation on time { loops: Animation.Infinite; from: 0; to: Math.PI * 2; duration: 600 }
            fragmentShader:
                "uniform highp float amplitude;" +
                "uniform highp float frequency;" +
                "uniform highp float time;" +
                "uniform sampler2D source;" +
                "varying highp vec2 qt_TexCoord0;" +
                "void main() {" +
                "    highp vec2 p = sin(time + frequency * qt_TexCoord0);" +
                "    gl_FragColor = texture2D(source, qt_TexCoord0 + amplitude * vec2(p.y, -p.x));" +
                "}"
            Slider {
                id: wobbleSlider
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 40
            }
        }
        ShaderEffectItem {
            width: 180
            height: 180
            property variant source: theSource
            property variant shadow: ShaderEffectSource {
                smooth: true
                sourceItem: ShaderEffectItem {
                    width: theItem.width
                    height: theItem.height
                    property variant delta: Qt.size(0.0, 1.0 / height)
                    property variant source: ShaderEffectSource {
                        smooth: true
                        sourceItem: ShaderEffectItem {
                            width: theItem.width
                            height: theItem.height
                            property variant delta: Qt.size(1.0 / width, 0.0)
                            property variant source: theSource
                            fragmentShader: "
                                uniform sampler2D source;
                                uniform highp vec2 delta;
                                varying highp vec2 qt_TexCoord0;
                                void main() {
                                    gl_FragColor = 0.0538 * texture2D(source, qt_TexCoord0 - 3.182 * delta)
                                                 + 0.3229 * texture2D(source, qt_TexCoord0 - 1.364 * delta)
                                                 + 0.2466 * texture2D(source, qt_TexCoord0)
                                                 + 0.3229 * texture2D(source, qt_TexCoord0 + 1.364 * delta)
                                                 + 0.0538 * texture2D(source, qt_TexCoord0 + 3.182 * delta);
                                }"
                        }
                    }
                    fragmentShader: "
                        uniform sampler2D source;
                        uniform highp vec2 delta;
                        varying highp vec2 qt_TexCoord0;
                        void main() {
                            gl_FragColor = 0.0538 * texture2D(source, qt_TexCoord0 - 3.182 * delta)
                                         + 0.3229 * texture2D(source, qt_TexCoord0 - 1.364 * delta)
                                         + 0.2466 * texture2D(source, qt_TexCoord0)
                                         + 0.3229 * texture2D(source, qt_TexCoord0 + 1.364 * delta)
                                         + 0.0538 * texture2D(source, qt_TexCoord0 + 3.182 * delta);
                        }"
                }
            }
            property real angle: 0
            property variant offset: Qt.point(15.0 * Math.cos(angle), 15.0 * Math.sin(angle))
            NumberAnimation on angle { loops: Animation.Infinite; from: 0; to: Math.PI * 2; duration: 6000 }
            property variant delta: Qt.size(offset.x / width, offset.y / height)
            property real darkness: shadowSlider.value
            fragmentShader: "
                uniform highp vec2 offset;
                uniform sampler2D source;
                uniform sampler2D shadow;
                uniform highp float darkness;
                uniform highp vec2 delta;
                varying highp vec2 qt_TexCoord0;
                void main() {
                    lowp vec4 fg = texture2D(source, qt_TexCoord0);
                    lowp vec4 bg = texture2D(shadow, qt_TexCoord0 + delta);
                    gl_FragColor = fg + vec4(0., 0., 0., darkness * bg.a) * (1. - fg.a);
                }"
            Slider {
                id: shadowSlider
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 40
            }
        }
        ShaderEffectItem {
            width: 180
            height: 180
            property variant source: theSource
            property variant delta: Qt.size(0.5 / width, 0.5 / height)
            fragmentShader: "
                uniform sampler2D source;
                uniform highp vec2 delta;
                uniform highp float qt_Opacity;
                varying highp vec2 qt_TexCoord0;
                void main() {
                    lowp vec4 tl = texture2D(source, qt_TexCoord0 - delta);
                    lowp vec4 tr = texture2D(source, qt_TexCoord0 + vec2(delta.x, -delta.y));
                    lowp vec4 bl = texture2D(source, qt_TexCoord0 - vec2(delta.x, -delta.y));
                    lowp vec4 br = texture2D(source, qt_TexCoord0 + delta);
                    lowp vec4 gx = (tl + bl) - (tr + br);
                    lowp vec4 gy = (tl + tr) - (bl + br);
                    gl_FragColor.xyz = vec3(0.);
                    gl_FragColor.w = clamp(dot(sqrt(gx * gx + gy * gy), vec4(1.)), 0., 1.) * qt_Opacity;
                }"
        }
        ShaderEffectItem {
            width: 180
            height: 180
            property variant source: theSource
            property color tint: sliderToColor(colorizeSlider.value)
            fragmentShader: "
                uniform sampler2D source;
                uniform lowp vec4 tint;
                uniform lowp float qt_Opacity;
                varying highp vec2 qt_TexCoord0;
                void main() {
                    lowp vec4 c = texture2D(source, qt_TexCoord0);
                    lowp float lo = min(min(c.x, c.y), c.z);
                    lowp float hi = max(max(c.x, c.y), c.z);
                    gl_FragColor = qt_Opacity * vec4(mix(vec3(lo), vec3(hi), tint.xyz), c.w);
                }"
            Slider {
                id: colorizeSlider
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 40
            }
        }
        ShaderEffectItem {
            width: 180
            height: 180
            mesh: GridMesh { resolution: Qt.size(10, 10) }
            property variant source: theSource
            property real bend: 0
            property real minimize: 0
            property real side: genieSlider.value
            SequentialAnimation on bend {
                loops: Animation.Infinite
                NumberAnimation { to: 1; duration: 700; easing.type: Easing.InOutSine }
                PauseAnimation { duration: 1600 }
                NumberAnimation { to: 0; duration: 700; easing.type: Easing.InOutSine }
                PauseAnimation { duration: 1000 }
            }
            SequentialAnimation on minimize {
                loops: Animation.Infinite
                PauseAnimation { duration: 300 }
                NumberAnimation { to: 1; duration: 700; easing.type: Easing.InOutSine }
                PauseAnimation { duration: 1000 }
                NumberAnimation { to: 0; duration: 700; easing.type: Easing.InOutSine }
                PauseAnimation { duration: 1300 }
            }
            vertexShader: "
                uniform highp mat4 qt_ModelViewProjectionMatrix;
                uniform highp float bend;
                uniform highp float minimize;
                uniform highp float side;
                uniform highp float width;
                uniform highp float height;
                attribute highp vec4 qt_Vertex;
                attribute highp vec2 qt_MultiTexCoord0;
                varying highp vec2 qt_TexCoord0;
                void main() {
                    qt_TexCoord0 = qt_MultiTexCoord0;
                    highp vec4 pos = qt_Vertex;
                    pos.y = mix(qt_Vertex.y, height, minimize);
                    highp float t = pos.y / height;
                    t = (3. - 2. * t) * t * t;
                    pos.x = mix(qt_Vertex.x, side * width, t * bend);
                    gl_Position = qt_ModelViewProjectionMatrix * pos;
                }"
            Slider {
                id: genieSlider
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 40
            }
        }
    }
}
