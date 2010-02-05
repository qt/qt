import Qt 4.6

Item {
    signal clicked

    states: [
        State { name: "X"; PropertyChanges { target: image; source: "pics/x.png" } },
        State { name: "O"; PropertyChanges { target: image; source: "pics/o.png" } }
    ]

    Image {
        id: image
        anchors.centerIn: parent
    }

    MouseRegion {
        anchors.fill: parent
        onClicked: parent.clicked()
    }
}
