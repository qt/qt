/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
                if (theSource.mipmap && theSource.smooth){
                    theSource.smooth = false
                } else if (theSource.mipmap && !theSource.smooth){
                    theSource.smooth = true
                    theSource.mipmap = false
                } else if (!theSource.mipmap && theSource.smooth){
                    theSource.smooth = false
                } else if (!theSource.mipmap && !theSource.smooth){
                    theSource.smooth = true
                    theSource.mipmap = true
                }
        }
    }

    ShaderEffectSource {
        id: theSource
        sourceImage: "wallpaper.jpg"
        mipmap: false
        smooth: false
    }

    ShaderEffectItem {
        id: effect
        anchors.centerIn: parent;
        width: parent.width * 0.8
        height: parent.height * 0.8
        property variant source: theSource
    }

    Rectangle {
        width: parent.width
        height: 40
        color: "#cc000000"

        Text {
             id: label
             anchors.centerIn:  parent
             text: "Mipmap: " + theSource.mipmap + ",  Smooth: " + theSource.smooth
             color: "white"
             font.bold: true
        }
    }
}
