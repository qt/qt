import Qt 4.7

Rectangle {
    id: myWin
    width: 500
    height: 400

    Image {
        id: image1
        source: "images/qt-logo.png"
        width: 150; height: 150
        anchors.bottom: myWin.bottom
        anchors.horizontalCenter: myWin.horizontalCenter
        anchors.bottomMargin: 10

        transform: Rotation {
            origin.x: 75; origin.y: 75
            axis{ x: 0; y: 0; z:1 }  angle: -90
        }

    }

    Text {
        text: "<h2>The Qt Logo -- taking it easy</h2>"
        anchors.bottom: image1.top
        anchors.horizontalCenter: myWin.horizontalCenter
        anchors.bottomMargin: 15

        transform: [
            Scale { xScale: 1.5; yScale: 1.2 } ,

            Rotation {
                origin.x: 75; origin.y: 75
                axis{ x: 0; y: 0; z:1 }  angle: -45
            }
        ]
    }
}
