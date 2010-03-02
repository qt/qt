import Qt 4.6

Rectangle {
    property alias day: dayText.text
    property var stickies

    id: page
    width: 400; height: 500; radius: 7
    border.color: "black"

    Image { x: 10; y: 10; source: "cork.jpg" }

    Text {
        id: dayText; x: 20; y: 20
        height: 40; width: 370
        font.pointSize: 14; font.bold: true
        style: Text.Outline; styleColor: "#dedede"
    }

    Repeater {
        model: page.stickies

        Item {
            id: stickyPage
            x: Math.random() * 200 + 100
            y: Math.random() * 300 + 50
            SpringFollow on rotation {
                source: -flickable.horizontalVelocity / 100
                spring: 2.0; damping: 0.1
            }

            Item {
                id: sticky
                scale: 0.5
                Image {
                    id: stickyImage; source: "sticky.png"
                    smooth: true; y: -20; x: 8 + -width * 0.6 / 2; scale: 0.6
                }

                TextEdit {
                    id: myText; smooth: true; font.pointSize: 28
                    readOnly: false; x: -104; y: 36; wrap: true
                    rotation: -8; text: noteText; width: 195; height: 172
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
                        drag.target: stickyPage; drag.axis: "XandYAxis"; drag.minimumY: 0; drag.maximumY: 500
                        drag.minimumX: 0; drag.maximumX: 400
                    }
                }
            }

            Image {
                source: "tack.png"
                x: -width / 2; y: -height * 0.7 / 2; scale: 0.7
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
