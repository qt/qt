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
