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

ShaderEffectItem {
    id: effect

    property real wave: 0.3
    property real waveOriginX: 0.5
    property real waveOriginY: 0.5
    property real waveWidth: 0.01
    property real aspectRatio: width/height
    property variant source: 0

    fragmentShader:
        "
        varying mediump vec2 qt_TexCoord0;
        uniform sampler2D source;
        uniform highp float wave;
        uniform highp float waveWidth;
        uniform highp float waveOriginX;
        uniform highp float waveOriginY;
        uniform highp float aspectRatio;

        void main(void)
        {
        mediump vec2 texCoord2 = qt_TexCoord0;
        mediump vec2 origin = vec2(waveOriginX, (1.0 - waveOriginY) / aspectRatio);

        highp float fragmentDistance = distance(vec2(texCoord2.s, texCoord2.t / aspectRatio), origin);
        highp float waveLength = waveWidth + fragmentDistance * 0.25;

        if ( fragmentDistance > wave && fragmentDistance < wave + waveLength) {
            highp float distanceFromWaveEdge = min(abs(wave - fragmentDistance), abs(wave + waveLength - fragmentDistance));
            texCoord2 += sin(1.57075 * distanceFromWaveEdge / waveLength) * distanceFromWaveEdge * 0.08 / fragmentDistance;
        }

        gl_FragColor = texture2D(source, texCoord2.st);
        }
        "
}
