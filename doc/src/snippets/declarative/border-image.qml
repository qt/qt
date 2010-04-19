import Qt 4.7

Rectangle {
    id: page
    color: "white"
    width: 520; height: 280

    Row {
        anchors.centerIn: parent
        spacing: 50
//! [0]
BorderImage {
    width: 180; height: 180
    border { left: 30; top: 30; right: 30; bottom: 30 }
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
    source: "content/colors.png"
}

BorderImage {
    width: 180; height: 180
    border { left: 30; top: 30; right: 30; bottom: 30 }
    horizontalTileMode: BorderImage.Round
    verticalTileMode: BorderImage.Round
    source: "content/colors.png"
}
//! [0]
    }
}
