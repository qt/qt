import Qt 4.6

Component {
    id: ListDelegate
    Item {
        id: Wrapper; width: Wrapper.ListView.view.width; height: 86
        Item {
            id: MoveMe
            Rectangle { color: "black"; opacity: index % 2 ? 0.2 : 0.4; height: 84; width: Wrapper.width; y: 1 }
            Rectangle {
                id: WhiteRect; x: 6; y: 4; width: 77; height: 77; color: "white"; smooth: true

                Image { id: Thumb; source: imagePath; x: 1; y: 1 }
                Image { source: "images/gloss.png" }
            }
            Column {
                x: 92; width: Wrapper.ListView.view.width - 95; y: 15; spacing: 2
                Text { text: title; color: "white"; width: parent.width; font.bold: true; elide: "ElideRight"; style: "Raised"; styleColor: "black" }
                Text { text: photoAuthor; color: "white"; width: parent.width; elide: "ElideLeft"; color: "#cccccc"; style: "Raised"; styleColor: "black" }
                Text { text: photoDate; color: "white"; width: parent.width; elide: "ElideRight"; color: "#cccccc"; style: "Raised"; styleColor: "black" }
            }
        }
    }
}
