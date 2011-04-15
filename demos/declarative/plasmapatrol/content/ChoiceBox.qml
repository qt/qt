/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
import Qt.labs.particles 2.0

Item{
    id: container
    width: 360
    height: 160
    property ParticleSystem system
    Ship{
        id: nully
        system: system
    }
    property Item target: nully
    /*
    Component.onCompleted:{
        container.target.shipType = 1
        container.target.gunType = 1
    }
    */
    Row{
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height
        spacing: 8
        Button{
            width: 80
            height: 80
            anchors.verticalCenter: parent.verticalCenter
            text: "Cycle\nShip"
            onClicked: {
                var nextVal = container.target.shipType;
                if(nextVal == 3)
                    nextVal = 1;
                else 
                    nextVal++;
                container.target.shipType = nextVal;
            }
        }
        Item{
            width: 128
            height: 128
            anchors.verticalCenter: parent.verticalCenter
            Ship{
                hp: 20
                anchors.centerIn: parent
                shipType: container.target.shipType
                gunType: container.target.gunType
                system: container.system
            }
        }
        Button{
            width: 80
            height: 80
            anchors.verticalCenter: parent.verticalCenter
            text: "Cycle\nGun"
            onClicked: {
                var nextVal = container.target.gunType;
                if(nextVal == 3)
                    nextVal = 1;
                else 
                    nextVal++;
                container.target.gunType = nextVal;
            }
        }
    }
}
