import Qt 4.6
import "../clock"

Rectangle {
    width: 320
    height: 480

    ParallaxView {
        anchors.fill: parent
        background: "pics/background.jpg"

        Row {
            Item {
                width: 320
                height: 480

                Clock {
                    anchors.centerIn: parent
                }
            }

            Item {
                width: 320
                height: 480

                Smiley {}
            }

            Item {
                width: 320
                height: 480

                Loader {
                    anchors.centerIn: parent
                    width: 300; height: 460
                    clip: true
                    resizeMode: Loader.SizeItemToLoader
                    
                    source: "../../../demos/declarative/samegame/samegame.qml"
                }
            }

        }
    }

}
