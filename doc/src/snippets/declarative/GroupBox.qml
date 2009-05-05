ContentWrapper {
    id: Container; width: parent.width
    Rect {
        width: parent.width; color: "white"; pen.width: 2; pen.color: "#adaeb0"; radius: 10
        clip: false; height: contents.height
        VerticalLayout {
            id: layout; width: parent.width
            Content {}
        }
    }
}
