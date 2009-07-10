Rect {
    property string day
    property var stickies

    width: 400
    height: 500
    radius: 7
    pen.color: "black"
    id: Page
    Image {
        x: 10
        y: 10
        source: "cork.jpg"
        opaque: true
    }
    Text {
        x: 20
        y: 20
        height: 40
        font.size: 14
        font.bold: true
        width: 370
        text: day
        style: Outline
        styleColor: "#dedede"
    }
    Repeater {
        dataSource: Page.stickies
        Item {
            x: Math.random() * 200 + 100
            y: Math.random() * 300 + 50
            id: StickyPage
            rotation: Follow {
                source: -Flick.xVelocity / 100
                spring: 2.0
                damping: 0.1
            }
            Item {
                id: Sticky
                scale: 0.5
                Image {
                    id: StickyImage
                    source: "sticky.png"
                    smooth: true
                    y: -20
                    x: 8 + -width * 0.6 / 2
                    scale: 0.6
                }
                TextEdit {
                    id: MyText
                    smooth: true
                    font.size: 28
                    readOnly: false
                    x: -104
                    y: 36
                    wrap: true
                    rotation: -8
                    text: noteText
                    width: 195
                    height: 172
                }
                Item {
                    y: -20
                    x: StickyImage.x
                    width: StickyImage.width * StickyImage.scale
                    height: StickyImage.height * StickyImage.scale
                    MouseRegion {
                        id: Mouse
                        onClicked: { MyText.focus = true }
                        anchors.fill: parent
                        drag.target: StickyPage
                        drag.axis: "xy"
                        drag.ymin: 0
                        drag.ymax: 500
                        drag.xmin: 0
                        drag.xmax: 400
                    }
                }
            }
            Image {
                source: "tack.png"
                x: -width / 2
                y: -height * 0.7 / 2
                scale: 0.7
            }
            states: [
                State {
                    name: "pressed"
                    when: Mouse.pressed
                    SetProperties {
                        target: Sticky
                        rotation: 8
                        scale: 1
                    }
                    SetProperties {
                        target: Page
                        z: 8
                    }
                }
            ]
            transitions: [
                Transition {
                    NumberAnimation {
                        properties: "rotation,scale"
                        duration: 200
                    }
                }
            ]
        }
    }
}
