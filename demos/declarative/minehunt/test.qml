import Qt 4.6

    Image {
                    source: "pics/front.png"
                    width: 40
                    height: 40
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        source: "pics/flag.png"
                        opacity: 1
                    }
                }
