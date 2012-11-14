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
    id: root
    property alias sourceItem: effectsource.sourceItem
    property real intensity: 1
    property bool waving: true
    anchors.top: sourceItem.bottom
    width: sourceItem.width
    height: sourceItem.height

    ShaderEffectItem {
        anchors.fill: parent
        property variant source: effectsource
        property real f: 0
        property real f2: 0
        property alias intensity: root.intensity
        smooth: true

        ShaderEffectSource {
            id: effectsource
            hideSource: false
            smooth: true
        }

        fragmentShader:
            "
            varying highp vec2 qt_TexCoord0;
            uniform sampler2D source;
            uniform lowp float qt_Opacity;
            uniform highp float f;
            uniform highp float f2;
            uniform highp float intensity;

            void main() {
                const highp float twopi = 3.141592653589 * 2.0;

                highp float distanceFactorToPhase = pow(qt_TexCoord0.y + 0.5, 8.0) * 5.0;
                highp float ofx = sin(f * twopi + distanceFactorToPhase) / 100.0;
                highp float ofy = sin(f2 * twopi + distanceFactorToPhase * qt_TexCoord0.x) / 60.0;

                highp float intensityDampingFactor = (qt_TexCoord0.x + 0.1) * (qt_TexCoord0.y + 0.2);
                highp float distanceFactor = (1.0 - qt_TexCoord0.y) * 4.0 * intensity * intensityDampingFactor;

                ofx *= distanceFactor;
                ofy *= distanceFactor;

                highp float x = qt_TexCoord0.x + ofx;
                highp float y = 1.0 - qt_TexCoord0.y + ofy;

                highp float fake = (sin((ofy + ofx) * twopi) + 0.5) * 0.05 * (1.2 - qt_TexCoord0.y) * intensity * intensityDampingFactor;

                highp vec4 pix =
                    texture2D(source, vec2(x, y)) * 0.6 +
                    texture2D(source, vec2(x-fake, y)) * 0.15 +
                    texture2D(source, vec2(x, y-fake)) * 0.15 +
                    texture2D(source, vec2(x+fake, y)) * 0.15 +
                    texture2D(source, vec2(x, y+fake)) * 0.15;

                highp float darken = 0.6 - (ofx - ofy) / 2.0;
                pix.b *= 1.2 * darken;
                pix.r *= 0.9 * darken;
                pix.g *= darken;

                gl_FragColor = qt_Opacity * vec4(pix.r, pix.g, pix.b, 1.0);
            }
            "

        NumberAnimation on f {
            running: root.waving
            loops: Animation.Infinite
            from: 0
            to: 1
            duration: 2410
        }
        NumberAnimation on f2 {
            running: root.waving
            loops: Animation.Infinite
            from: 0
            to: 1
            duration: 1754
        }
    }
}
