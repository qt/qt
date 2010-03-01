import Qt 4.6

Item {
    id: button
    width: 48; height: 48

    property alias image: icon.source
    signal clicked

    Image { id: icon }
    MouseArea { anchors.fill: icon; onClicked: button.clicked() }
}
