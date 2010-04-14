import Qt 4.7

Rectangle {
    width: 400; height: 200

    Image {
        id: blur
        x: 5
        source: "pic.png"

        effect: Blur {
            NumberAnimation on blurRadius {
                id: blurEffect
                running: false
                from: 0; to: 10
                duration: 1000
                loops: Animation.Infinite
            }
        }

        MouseArea { anchors.fill: parent; onClicked: blurEffect.running = !blurEffect.running }
    }

    Text { text: "Blur"; anchors.top: blur.bottom; anchors.horizontalCenter: blur.horizontalCenter }

    Image {
        id: dropShadow
        source: "pic.png"
        x: 135

        effect: DropShadow {
            blurRadius: 3
            offset.x: 3

            NumberAnimation on offset.y {
                id: dropShadowEffect
                from: 0; to: 10
                duration: 1000
                running: false
                loops: Animation.Infinite
            }
        }

        MouseArea { anchors.fill: parent; onClicked: dropShadowEffect.running = !dropShadowEffect.running }
    }

    Image {
        id: colorize
        source: "pic.png"
        x: 265

        effect: Colorize { color: "blue" }
    }

    Text { text: "Colorize"; anchors.top: colorize.bottom; anchors.horizontalCenter: colorize.horizontalCenter }

    Text { text: "Drop Shadow"; anchors.top: dropShadow.bottom; anchors.horizontalCenter: dropShadow.horizontalCenter }

    Text {
        y: 155; anchors.horizontalCenter: parent.horizontalCenter
        text: "Clicking Blur or Drop Shadow will \ntoggle animation."
        color: "black"
    }

}
