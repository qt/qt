import Qt 4.6

ContentWrapper {
    id: container; width: parent.width; height: contents.height
    children: [
        Rectangle {
            width: parent.width; height: contents.height
            color: "white"; pen.width: 2; pen.color: "#adaeb0"; radius: 10
            Column {
                id: layout; width: parent.width; margin: 5; spacing: 2
                Content { }
            }
        }
    ]
}
