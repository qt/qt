import Qt 4.6

Rectangle {
    color: "white"
    width: 600
    height: 600

    Image {
        id: blur
        source: "pic.png"

        effect: Blur {
            blurRadius: NumberAnimation {
                id: blurEffect
                from: 0; to: 10
                duration: 1000
                repeat: true
            }
        }

        MouseRegion { anchors.fill: parent; onClicked: blurEffect.running = !blurEffect.running }
    }

    Text { text: "Blur"; anchors.top: blur.bottom; anchors.horizontalCenter: blur.horizontalCenter }

    Image {
        id: grayscale
        source: "pic.png"
        x: 200

        effect: Grayscale {}
    }

    Text { text: "Grayscale"; anchors.top: grayscale.bottom; anchors.horizontalCenter: grayscale.horizontalCenter }

    Image {
        id: colorize
        source: "pic.png"
        x: 400

        effect: Colorize { color: "blue" }
    }

    Text { text: "Colorize"; anchors.top: colorize.bottom; anchors.horizontalCenter: colorize.horizontalCenter }

    Image {
        id: pixelize
        source: "pic.png"
        y: 300

        effect: Pixelize {
            pixelSize: NumberAnimation {
                id: pixelizeEffect
                from: 0; to: 10
                duration: 1000
                repeat: true
            }
        }

        MouseRegion { anchors.fill: parent; onClicked: pixelizeEffect.running = !pixelizeEffect.running }
    }

    Text { text: "Pixelize"; anchors.top: pixelize.bottom; anchors.horizontalCenter: pixelize.horizontalCenter }

    Image {
        id: dropShadow
        source: "pic.png"
        x: 200
        y: 300

        effect: DropShadow {
            blurRadius: 3
            offset.x: 3
            offset.y: NumberAnimation { id: dropShadowEffect; from: 0; to: 10; duration: 1000; repeat: true; }
        }

        MouseRegion { anchors.fill: parent; onClicked: dropShadowEffect.running = !dropShadowEffect.running }
    }

    Text { text: "Drop Shadow"; anchors.top: dropShadow.bottom; anchors.horizontalCenter: dropShadow.horizontalCenter }

    Image {
        id: bloom
        source: "pic.png"
        x: 400
        y: 300

        effect: Bloom {
            blurRadius: 3
            brightness: 128
            strength: NumberAnimation {
                id: bloomEffect
                from: 0; to: 1
                duration: 1000
                repeat: true
            }
        }

        MouseRegion { anchors.fill: parent; onClicked: bloomEffect.running = !bloomEffect.running }
    }

    Text { text: "Bloom"; anchors.top: bloom.bottom; anchors.horizontalCenter: bloom.horizontalCenter }

    Text {
        x: 100; y: 250
        text: "Clicking Blur, Pixelize, Drop Shadow or Bloom will \ntoggle animation."
        color: "black"
    }

}
