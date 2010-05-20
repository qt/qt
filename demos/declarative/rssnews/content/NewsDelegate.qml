import Qt 4.7

Item {
    id: delegate
    height: childrenRect.height + 20
    width: delegate.ListView.view.width

    Column {
        x: 20; y: 20
        width: parent.width - 40

        Text {
            id: titleText
            text: title; width: parent.width; wrapMode: Text.WordWrap
            font { bold: true; family: "Helvetica"; pointSize: 16 }
        }

        Text {
            id: descriptionText
            width: parent.width; text: description
            wrapMode: Text.WordWrap; font.family: "Helvetica"
        }
    }

    Rectangle {
        width: parent.width; height: 1; color: "#cccccc"
        anchors.bottom: parent.bottom
    }
}
