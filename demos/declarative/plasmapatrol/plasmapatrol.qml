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
import "content"

Rectangle {
    id: root
    property bool readySetGo: false
    //Debugging
    property int hits: 0
    property int misses: 0
    property real ratio: hits/(misses?misses:1)
    //Move to JS file?
    property Ship redVar1: Ship{shipType: 1; gunType: 1}
    property Ship redVar2: Ship{shipType: 1; gunType: 2}
    property Ship redVar3: Ship{shipType: 1; gunType: 3}
    property Ship greenVar1: Ship{shipType: 3; gunType: 1}
    property Ship greenVar2: Ship{shipType: 2; gunType: 2}
    property Ship greenVar3: Ship{shipType: 1; gunType: 3}
    property string winner: "0"
    property int players: 0
    function aiSet(ship){
        ship.gunType = Math.floor(Math.random() * 3) + 1
        ship.shipType = Math.floor(Math.random() * 3) + 1
    }

    width: 360
    height: 600
    color: "black"
    SequentialLoader{
        anchors.fill: parent
        //General Children
        Image{
            anchors.centerIn: parent
            source: "content/pics/finalfrontier.png"
        }
        ParticleSystem{
            id: particles
        }
        PlasmaPatrolParticles{ sys: particles; z: 100 }//Renders all particles on the one plane
        //Component parts
        id: pageControl
        Component.onCompleted: advance();
        pages:[
        Component{Item{
            id: menu
            width: root.width
            height: root.height
            Column{
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                spacing: 8
                Item{
                    id: title
                    width: root.width
                    height: 240
                    TrailEmitter{
                        anchors.fill: parent
                        system: particles
                        emitting: true
                        particle: "default"
                        particlesPerSecond: 1200
                        particleDuration: 1200
                        shape: Mask{source:"content/pics/TitleText.png"}
                        particleSize: 16
                        particleEndSize: 0
                        particleSizeVariation: 8
                        speed: AngleVector{angleVariation:360; magnitudeVariation: 6}
                    }
                }
                Button{
                    text: "1P"
                    onClicked: {root.players = 1; pageControl.advance();}
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Button{
                    text: "2P"
                    onClicked: {root.players = 2; pageControl.advance();}
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Button{
                    text: "Demo"
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {root.players = 0; 
                        aiSet(redVar1);
                        aiSet(redVar2);
                        aiSet(redVar3);
                        aiSet(greenVar1);
                        aiSet(greenVar2);
                        aiSet(greenVar3);
                        pageControl.at = 5;//TODO: Not a magic number
                        pageControl.advance();}
                }
                Button{
                    text: "Help"
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        pageControl.at = 7;//TODO: Not a magic number
                        pageControl.advance();
                    }
                }
                Button{
                    text: "Quit"
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: Qt.quit();
                }
            }
        }},
        Component{Item{
            id: p1Screen
            z: 101
            width: root.width
            height: root.height
            Rectangle{
                anchors.fill: parent
                color: "red"
            }
            Text{
                anchors.centerIn: parent
                color: "white"
                font.pixelSize: 64
                font.bold: true
                text: "Player\n    1"
                horizontalAlignment: Text.AlignHCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked: pageControl.advance()
            }
        }},
        Component{Item{
            id: p1Choices
            z: 3
            width: root.width
            height: root.height
            Rectangle{
                color: "black"
                anchors.fill: parent
            }
            Column{
                spacing: 16
                width: root.width
                anchors.horizontalCenter: parent.horizontalCenter
                ChoiceBox{
                    target: redVar1
                    system: particles
                }
                ChoiceBox{
                    target: redVar2
                    system: particles
                }
                ChoiceBox{
                    target: redVar3
                    system: particles
                }
                Button{
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Next"
                    onClicked: {
                        if(root.players < 2){
                            aiSet(greenVar1);
                            aiSet(greenVar2);
                            aiSet(greenVar3);
                            pageControl.at = 5;//TODO: Not a magic number
                        }
                        pageControl.advance();
                    }
                }
            }
        }},
        Component{Item{
            id: p2Screen
            z: 101
            width: root.width
            height: root.height
            Rectangle{
                anchors.fill: parent
                color: "green"
            }
            Text{
                anchors.centerIn: parent
                color: "white"
                font.pixelSize: 64
                font.bold: true
                text: "Player\n    2"
                horizontalAlignment: Text.AlignHCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked: pageControl.advance()
            }
        }},
        Component{Item{
            id: p2Choices
            z: 1
            width: root.width
            height: root.height
            Rectangle{
                color: "black"
                anchors.fill: parent
            }
            Column{
                spacing: 16
                width: root.width
                anchors.horizontalCenter: parent.horizontalCenter
                ChoiceBox{
                    target: greenVar1
                    system: particles
                }
                ChoiceBox{
                    target: greenVar2
                    system: particles
                }
                ChoiceBox{
                    target: greenVar3
                    system: particles
                }
                Button{
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Next"
                    onClicked: pageControl.advance()
                }
            }
        }},
        Component{Item{
            id: arena
            width: root.width
            height: root.height
            z: 0
            Component.onCompleted: root.readySetGo = true
            Component.onDestruction: root.readySetGo = false
            property bool victory: redShip3.hp <= 0 || greenShip3.hp <=0
            onVictoryChanged: {
                if(redShip3.hp <= 0){
                    if(greenShip3.hp <= 0){
                        root.winner = "1&2"
                    }else{
                        root.winner = "2"
                    }
                }else{
                    root.winner = "1"
                }
                winTimer.start()
            }
            Timer{
                id: winTimer
                interval: 1200
                repeat: false
                running: false
                onTriggered: pageControl.advance();
            }
            Ship{
                id: redShip1
                shipParticle: "redTeam"
                system: particles
                x: 180-64
                y: 128
                shipType: redVar1.shipType
                gunType: redVar1.gunType
                targets: [greenShip1, greenShip2, greenShip3]
            }
            Ship{
                id: redShip2
                shipParticle: "redTeam"
                system: particles
                x: 0
                y: 0
                shipType: redVar2.shipType
                gunType: redVar2.gunType
                targets: [greenShip1, greenShip2, greenShip3]
            }
            Ship{
                id: redShip3
                shipParticle: "redTeam"
                system: particles
                x: 360-128
                y: 0
                shipType: redVar3.shipType
                gunType: redVar3.gunType
                targets: [greenShip1, greenShip2, greenShip3]
            }

            Ship{
                id: greenShip1
                shipParticle: "greenTeam"
                system: particles
                x: 180-64
                y: 600 - 128 - 128
                shipType: greenVar1.shipType
                gunType: greenVar1.gunType
                targets: [redShip1, redShip2, redShip3]
            }
            Ship{
                id: greenShip2
                shipParticle: "greenTeam"
                system: particles
                x: 0
                y: 600-128
                shipType: greenVar2.shipType
                gunType: greenVar2.gunType
                targets: [redShip1, redShip2, redShip3]
            }
            Ship{
                id: greenShip3
                shipParticle: "greenTeam"
                system: particles
                x: 360 - 128
                y: 600 - 128
                shipType: greenVar3.shipType
                gunType: greenVar3.gunType
                targets: [redShip1, redShip2, redShip3]
            }
        }},
        Component{Item{
            id: winScreen
            z: 101
            width: root.width
            height: root.height
            /*
            Rectangle{
                anchors.fill: parent
                color: "black"
            }
            */
            Text{//TODO: Particle Text?
                anchors.fill: parent
                color: "white"
                font.pixelSize: 64
                font.bold: true
                text: "Player " + root.winner + " wins!"
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {pageControl.at = 0; pageControl.advance();}
            }
        }},
        Component{
            HelpScreens{
                onExitDesired: {pageControl.at = 0; pageControl.advance();}
            }
        }
        ]
    }
}
