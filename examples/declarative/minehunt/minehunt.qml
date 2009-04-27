Item {
    id: field
    width: 370
    height: 480
    properties: Property {
        name: "clickx"
        type: "Int"
        value: 0
    }
    properties: Property {
        name: "clicky"
        type: "Int"
        value: 0
    }
    resources: [
        Component {
            id: tile
            Flipable {
                id: flipable
                width: 40
                height: 40
                transform: [
                    Axis {
                        id: axis
                        xStart: 20
                        xEnd: 20
                        yStart: 20
                        yEnd: 0
                    }
                ]
                front: Image {
                    src: "pics/front.png"
                    width: 40
                    height: 40
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        src: "pics/flag.png"
                        opacity: modelData.hasFlag
                        opacity: Behaviour {
                            NumericAnimation {
                                property: "opacity"
                                duration: 250
                            }
                        }
                    }
                }
                back: Image {
                    src: "pics/back.png"
                    width: 40
                    height: 40
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.hint
                        color: "white"
                        font.bold: true
                        opacity: modelData.hasMine == false && modelData.hint > 0
                    }
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        src: "pics/bomb.png"
                        opacity: modelData.hasMine
                    }
                    Explosion {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        explode: modelData.hasMine==true && modelData.flipped==true
                    }
                }
                states: [
                    State {
                        name: "back"
                        when: modelData.flipped == true
                        SetProperty {
                            target: axis
                            property: "rotation"
                            value: 180
                        }
                    }
                ]
                transitions: [
                    Transition {
                        SequentialAnimation {
                            PauseAnimation {
                                duration: {
                                      var ret = Math.abs(flipable.parent.x-field.clickx) +
                                                Math.abs(flipable.parent.y-field.clicky);

                                      ret > 0
                                        ? (modelData.hasMine==true && modelData.flipped==true) ? ret * 3 : ret
                                        : 0
                                }
                            }
                            NumericAnimation {
                                easing: "easeInOutQuad"
                                properties: "rotation"
                            }
                        }
                    }
                ]
                MouseRegion {
                    anchors.fill: parent
                    onClicked: {
                        field.clickx = flipable.parent.x;
                        field.clicky = flipable.parent.y;
                        row = Math.floor(index/9);
                        col = index - (Math.floor(index/9) * 9);
                        if (mouseButton==undefined || mouseButton=='Right') {
                            flag(row, col);
                        } else{
                            flip(row, col);
                        }
                    }
                }
            }
        }
    ]
    Image {
        src: "pics/No-Ones-Laughing-3.jpg"
        tile: true
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
            src: "pics/bomb-color.png"
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
            src: "pics/flag-color.png"
        }
        Text {
            x: 140
            y: 60
            color: "white"
            text: numFlags
        }
        Image {
            x: 240
            y: 0
            src: if(isPlaying==true){'pics/smile.png'}else{if(hasWon==true){'pics/glee.png'}else{'pics/frown.png'}}
            MouseRegion {
                anchors.fill: parent
                onClicked: { reset() }
            }
        }
    }
}
