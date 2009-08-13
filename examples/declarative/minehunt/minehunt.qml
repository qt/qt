import Qt 4.6

Item {
    id: field
    width: 370
    height: 480

    property int clickx : 0
    property int clicky : 0

    resources: [
        Component {
            id: tile
            Flipable {
                id: flipable
                width: 40
                height: 40
                property int angle: 0;
                transform: Rotation {
                    origin.x: 20
                    origin.y: 20
                    axis.x: 1
                    axis.z: 0
                    angle: flipable.angle;
                }
                front: Image {
                    source: "pics/front.png"
                    width: 40
                    height: 40
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        source: "pics/flag.png"
                        opacity: modelData.hasFlag
                        opacity: Behavior {
                            NumberAnimation {
                                property: "opacity"
                                duration: 250
                            }
                        }
                    }
                }
                back: Image {
                    source: "pics/back.png"
                    width: 40
                    height: 40
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.hint
                        color: "white"
                        font.bold: true
                        opacity: !modelData.hasMine && modelData.hint > 0
                    }
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        source: "pics/bomb.png"
                        opacity: modelData.hasMine
                    }
                    Explosion {
                        id: expl
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        //explode: modelData.hasMine && modelData.flipped//Doesn't wait for the pause
                    }
                }
                states: [
                    State {
                        name: "back"
                        when: modelData.flipped
                        SetProperties { target: flipable; angle: 180 }
                    }
                ]
                transitions: [
                    Transition {
                        SequentialAnimation {
                            PauseAnimation {
                                duration: {
                                    var ret = Math.abs(flipable.parent.x-field.clickx)
                                            + Math.abs(flipable.parent.y-field.clicky);
                                    if (ret > 0) {
                                        if (modelData.hasMine && modelData.flipped) {
                                            ret*3
                                        } else {
                                            ret
                                        }
                                    } else {
                                        0
                                    }
                                }
                            }
                            NumberAnimation {
                                easing: "easeInOutQuad"
                                properties: "angle"
                            }
                            RunScriptAction{
                                script: if(modelData.hasMine && modelData.flipped)
                                            {expl.explode = true;}
                            }
                        }
                    }
                ]
                MouseRegion {
                    anchors.fill: parent
                    onPressed: {
                        field.clickx = flipable.parent.x;
                        field.clicky = flipable.parent.y;
                        row = Math.floor(index/9);
                        col = index - (Math.floor(index/9) * 9);
                        if (mouse.button==undefined || mouse.button==Qt.RightButton) {
                            flag(row,col);
                        } else {
                            flip(row,col);
                        }
                    }
                }
            }
        }
    ]
    Image {
        source: "pics/No-Ones-Laughing-3.jpg"
        fillMode: "Tile"
    }
    Description {
        text: "Use the 'minehunt' executable to run this demo!"
        width: 300
        opacity: tiles?0:1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
    Repeater {
        dataSource: tiles
        x: 1
        y: 1
        Component {
            ComponentInstance {
                component: tile
                x: (index - (Math.floor(index/9) * 9)) * 41
                y: Math.floor(index/9) * 41
            }
        }
    }
    Item {
        id: gamedata
        width: 370
        height: 100
        y: 380
        Text {
            color: "white"
            font.size: 18
            x: 20
            y: 20
        }
        Image {
            x: 100
            y: 20
            source: "pics/bomb-color.png"
        }
        Text {
            x: 100
            y: 60
            color: "white"
            text: numMines
        }
        Image {
            x: 140
            y: 20
            source: "pics/flag-color.png"
        }
        Text {
            x: 140
            y: 60
            color: "white"
            text: numFlags
        }
        Image {
            x: 280
            y: 10
            source: isPlaying ? 'pics/face-smile.png' : hasWon ? 'pics/face-smile-big.png': 'pics/face-sad.png'
            MouseRegion {
                anchors.fill: parent
                onPressed: { reset() }
            }
        }
    }
}
