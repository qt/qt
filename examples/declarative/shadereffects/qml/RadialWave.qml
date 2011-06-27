/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

import QtQuick 1.0
import Qt.labs.shaders 1.0

Item {
    id: main
    anchors.fill: parent

    ShaderEffectSource {
        id: thesource
        sourceItem: Image { source: "images/wave.jpg" }
        live: false
        hideSource: true
    }

    RadialWaveEffect {
        id: layer
        anchors.fill: parent;
        source: thesource

        wave: 0.0
        waveOriginX: 0.5
        waveOriginY: 0.5
        waveWidth: 0.01

        NumberAnimation on wave {
            id: waveAnim
            running: true
            loops: Animation.Infinite
            easing.type: "Linear"
            from: 0.0000; to: 2.0000;
            duration: 2500
        }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            waveAnim.stop()
            layer.waveOriginX = mouseX / main.width
            layer.waveOriginY = mouseY / main.height
            waveAnim.start()
        }
    }
}
