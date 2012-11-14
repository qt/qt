/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import Qt.labs.shaders 1.0

// Note 1. This shader implements gaussian blur without dynamic array access from inside shader loops (Optional feature in OpenGLES 2.0).
// Note 2. Shader code is generated to avoid ecessive if-else structure in fragment shader. Code re-generation (very slow!) happens if blur radius is changed.

ShaderEffectItem {
    id: effect
    property variant source: 0
    property real deviation: Math.sqrt(-((radius+1) * (radius+1)) / (2 * Math.log(1.0 / 255.0)));
    property real radius: 8;
    property real deltaX: 0.0
    property real deltaY: 0.0

    property real gaussianSum: 0.0
    property real startIndex: 0.0
    property real samples: radius * 2

    property variant gwts: []
    property variant delta: Qt.vector3d(effect.deltaX, effect.deltaY, effect.startIndex);
    property variant factor_0_2: Qt.vector3d(effect.gwts[0], effect.gwts[1], effect.gwts[2]);
    property variant factor_3_5: Qt.vector3d(effect.gwts[3],effect.gwts[4],effect.gwts[5]);
    property variant factor_6_8: Qt.vector3d(effect.gwts[6],effect.gwts[7],effect.gwts[8]);
    property variant factor_9_11: Qt.vector3d(effect.gwts[9],effect.gwts[10],effect.gwts[11]);
    property variant factor_12_14: Qt.vector3d(effect.gwts[12],effect.gwts[13],effect.gwts[14]);
    property variant factor_15_17: Qt.vector3d(effect.gwts[15],effect.gwts[16],effect.gwts[17]);
    property variant factor_18_20: Qt.vector3d(effect.gwts[18],effect.gwts[19],effect.gwts[20]);
    property variant factor_21_23: Qt.vector3d(effect.gwts[21],effect.gwts[22],effect.gwts[23]);
    property variant factor_24_26: Qt.vector3d(effect.gwts[24],effect.gwts[25],effect.gwts[26]);
    property variant factor_27_29: Qt.vector3d(effect.gwts[27],effect.gwts[28],effect.gwts[29]);
    property variant factor_30_32: Qt.vector3d(effect.gwts[30],effect.gwts[31],effect.gwts[32]);

    //Gaussian function = h(x):=(1/sqrt(2*3.14159*(D^2))) * %e^(-(x^2)/(2*(D^2)));
    function gausFunc(x){
        return (1/Math.sqrt(2*3.1415926*(Math.pow(effect.deviation,2)))) * Math.pow(2.7182818,-((Math.pow(x,2))/(2*(Math.pow(effect.deviation,2)))));
    }

    function calcGWTS() {
        var n = new Array(Math.floor(effect.samples));
        var step
        for (var i = 0; i < effect.samples; i++) {
            step = -effect.samples/2 + i + 0.5
            n[i] = gausFunc(step);
        }
        return n;
    }

    function buildFragmentShader() {

        var shaderSteps = [
            "gl_FragColor += texture2D(source, texCoord) * factor_0_2.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_0_2.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_0_2.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_3_5.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_3_5.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_3_5.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_6_8.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_6_8.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_6_8.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_9_11.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_9_11.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_9_11.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_12_14.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_12_14.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_12_14.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_15_17.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_15_17.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_15_17.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_18_20.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_18_20.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_18_20.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_21_23.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_21_23.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_21_23.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_24_26.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_24_26.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_24_26.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_27_29.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_27_29.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_27_29.z; texCoord += shift;",

            "gl_FragColor += texture2D(source, texCoord) * factor_30_32.x; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_30_32.y; texCoord += shift;",
            "gl_FragColor += texture2D(source, texCoord) * factor_30_32.z; texCoord += shift;"
        ]

        var shader = fragmentShader_begin
        var samples = effect.samples
        if (samples > 32) {
            console.log("GaussianBlur: Maximum of 32 blur samples exceeded!")
            samples = 32
        }

        for (var i = 0; i < samples; i++) {
            shader += shaderSteps[i]
        }

        shader += fragmentShader_end
        effect.fragmentShader = shader

    }

    onDeviationChanged:{
        effect.startIndex = -effect.samples/2 + 0.5
        effect.gwts = calcGWTS();
        var sum = 0.0;
        for (var j = 0; j < effect.samples; j++) {
            sum += effect.gwts[j];
        }
        effect.gaussianSum = sum
    }

    Component.onCompleted:{
        effect.startIndex = -effect.samples/2 + 0.5
        effect.gwts = calcGWTS();
        var sum = 0.0;
        for (var j = 0; j < effect.samples; j++) {
            sum += effect.gwts[j];
        }
        effect.gaussianSum = sum
        buildFragmentShader()
    }

    onSamplesChanged: {
        buildFragmentShader()
    }

    property string fragmentShader_begin:
        "
        varying mediump vec2 qt_TexCoord0;
        uniform sampler2D source;
        uniform highp vec3 delta;
        uniform highp vec3 factor_0_2;
        uniform highp vec3 factor_3_5;
        uniform highp vec3 factor_6_8;
        uniform highp vec3 factor_9_11;
        uniform highp vec3 factor_12_14;
        uniform highp vec3 factor_15_17;
        uniform highp vec3 factor_18_20;
        uniform highp vec3 factor_21_23;
        uniform highp vec3 factor_24_26;
        uniform highp vec3 factor_27_29;
        uniform highp vec3 factor_30_32;
        uniform highp float gaussianSum;

        void main() {
            highp vec2 shift = vec2(delta.x, delta.y);
            highp float index = delta.z;
            mediump vec2 texCoord = qt_TexCoord0 + (shift * index);
            gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        "

    property string fragmentShader_end:
        "
            if (gaussianSum > 0.0)
                gl_FragColor /= gaussianSum;
        }
        "
}
