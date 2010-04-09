import Qt 4.7

Component {
    Item {
        property variant stickies

        id: page
        width: 840; height: 480

        Image { source: "cork.jpg" }

        Text {
            text: name; x: 15; y: 8; height: 40; width: 370
            font.pixelSize: 18; font.bold: true; color: "white"
            style: Text.Outline; styleColor: "black"
        }

        Repeater {
            model: notes
            Item {
                property int randomX: Math.random() * 500 + 100
                property int randomY: Math.random() * 200 + 50

                id: stickyPage
                x: randomX; y: randomY

                SpringFollow on rotation {
                    to: -flickable.horizontalVelocity / 100
                    spring: 2.0; damping: 0.15
                }

                Item {
                    id: sticky
                    scale: 0.7
                    Image {
                        id: stickyImage
                        source: "note-yellow.png"; transformOrigin: Item.TopLeft
                        smooth: true; y: -20; x: 8 + -width * 0.6 / 2; scale: 0.6
                    }

                    TextEdit {
                        id: myText; smooth: true; font.pixelSize: 24
                        readOnly: false; x: -104; y: 36
                        rotation: -8; text: noteText; width: 215; height: 200
                    }

                    Item {
                        y: -20
                        x: stickyImage.x
                        width: stickyImage.width * stickyImage.scale
                        height: stickyImage.height * stickyImage.scale
                        MouseArea {
                            id: mouse
                            onClicked: { myText.focus = true }
                            anchors.fill: parent
                            drag.target: stickyPage
                            drag.axis: "XandYAxis"
                            drag.minimumY: 0
                            drag.maximumY: page.height - 80
                            drag.minimumX: 100
                            drag.maximumX: page.width - 140
                        }
                    }
                }

                Image {
                    source: "tack.png"; transformOrigin: Item.TopLeft
                    x: -width / 2; y: -height * 0.5 / 2; scale: 0.7
                }

                states: State {
                    name: "pressed"
                    when: mouse.pressed
                    PropertyChanges { target: sticky; rotation: 8; scale: 1 }
                    PropertyChanges { target: page; z: 8 }
                }

                transitions: Transition {
                    NumberAnimation { properties: "rotation,scale"; duration: 200 }
                }
            }
        }
    }
}








