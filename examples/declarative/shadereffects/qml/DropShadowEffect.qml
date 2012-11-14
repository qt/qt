/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

Item {
    id: main
    property real blur: 0.0
    property alias color: shadowEffectWithHBlur.color
    property alias sourceItem: source.sourceItem

    ShaderEffectSource {
        id: source
        smooth: true
        hideSource: false
    }

    ShaderEffectItem {
        id: shadowEffectWithHBlur
        anchors.fill: parent

        property color color: "grey"
        property variant sourceTexture: source;
        property real xStep: main.blur / main.width

        vertexShader:"
            uniform highp mat4 qt_ModelViewProjectionMatrix;
            attribute highp vec4 qt_Vertex;
            attribute highp vec2 qt_MultiTexCoord0;
            uniform highp float xStep;
            varying highp vec2 qt_TexCoord0;
            varying highp vec2 qt_TexCoord1;
            varying highp vec2 qt_TexCoord2;
            varying highp vec2 qt_TexCoord4;
            varying highp vec2 qt_TexCoord5;
            varying highp vec2 qt_TexCoord6;

            void main(void)
            {
                highp vec2 shift = vec2(xStep, 0.);
                qt_TexCoord0 = qt_MultiTexCoord0 - 2.5 * shift;
                qt_TexCoord1 = qt_MultiTexCoord0 - 1.5 * shift;
                qt_TexCoord2 = qt_MultiTexCoord0 - 0.5 * shift;
                qt_TexCoord4 = qt_MultiTexCoord0 + 0.5 * shift;
                qt_TexCoord5 = qt_MultiTexCoord0 + 1.5 * shift;
                qt_TexCoord6 = qt_MultiTexCoord0 + 2.5 * shift;
                gl_Position =  qt_ModelViewProjectionMatrix * qt_Vertex;
            }
        "

        fragmentShader:"
            uniform highp vec4 color;
            uniform lowp sampler2D sourceTexture;
            varying highp vec2 qt_TexCoord0;
            varying highp vec2 qt_TexCoord1;
            varying highp vec2 qt_TexCoord2;
            varying highp vec2 qt_TexCoord4;
            varying highp vec2 qt_TexCoord5;
            varying highp vec2 qt_TexCoord6;

            void main() {
                highp vec4 sourceColor = (texture2D(sourceTexture, qt_TexCoord0) * 0.1
                + texture2D(sourceTexture, qt_TexCoord1) * 0.15
                + texture2D(sourceTexture, qt_TexCoord2) * 0.25
                + texture2D(sourceTexture, qt_TexCoord4) * 0.25
                + texture2D(sourceTexture, qt_TexCoord5) * 0.15
                + texture2D(sourceTexture, qt_TexCoord6) * 0.1);
                gl_FragColor = mix(vec4(0), color, sourceColor.a);
            }
        "
    }

    ShaderEffectSource {
        id: hBlurredShadow
        smooth: true
        sourceItem: shadowEffectWithHBlur
        hideSource: true
    }

    ShaderEffectItem {
        id: finalEffect
        anchors.fill: parent

        property color color: "grey"
        property variant sourceTexture: hBlurredShadow;
        property real yStep: main.blur / main.height

        vertexShader:"
            uniform highp mat4 qt_ModelViewProjectionMatrix;
            attribute highp vec4 qt_Vertex;
            attribute highp vec2 qt_MultiTexCoord0;
            uniform highp float yStep;
            varying highp vec2 qt_TexCoord0;
            varying highp vec2 qt_TexCoord1;
            varying highp vec2 qt_TexCoord2;
            varying highp vec2 qt_TexCoord4;
            varying highp vec2 qt_TexCoord5;
            varying highp vec2 qt_TexCoord6;

            void main(void)
            {
                highp vec2 shift = vec2(0., yStep);
                qt_TexCoord0 = qt_MultiTexCoord0 - 2.5 * shift;
                qt_TexCoord1 = qt_MultiTexCoord0 - 1.5 * shift;
                qt_TexCoord2 = qt_MultiTexCoord0 - 0.5 * shift;
                qt_TexCoord4 = qt_MultiTexCoord0 + 0.5 * shift;
                qt_TexCoord5 = qt_MultiTexCoord0 + 1.5 * shift;
                qt_TexCoord6 = qt_MultiTexCoord0 + 2.5 * shift;
                gl_Position =  qt_ModelViewProjectionMatrix * qt_Vertex;
            }
        "

        fragmentShader:"
            uniform highp vec4 color;
            uniform lowp sampler2D sourceTexture;
            uniform highp float qt_Opacity;
            varying highp vec2 qt_TexCoord0;
            varying highp vec2 qt_TexCoord1;
            varying highp vec2 qt_TexCoord2;
            varying highp vec2 qt_TexCoord4;
            varying highp vec2 qt_TexCoord5;
            varying highp vec2 qt_TexCoord6;

            void main() {
                highp vec4 sourceColor = (texture2D(sourceTexture, qt_TexCoord0) * 0.1
                + texture2D(sourceTexture, qt_TexCoord1) * 0.15
                + texture2D(sourceTexture, qt_TexCoord2) * 0.25
                + texture2D(sourceTexture, qt_TexCoord4) * 0.25
                + texture2D(sourceTexture, qt_TexCoord5) * 0.15
                + texture2D(sourceTexture, qt_TexCoord6) * 0.1);
                gl_FragColor = sourceColor * qt_Opacity;
            }
        "
    }
}
