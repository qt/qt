import Qt 4.7

Component {
    Item {
        id: wrapper; width: wrapper.ListView.view.width; height: 86
        Item {
            id: moveMe
            Rectangle { color: "black"; opacity: index % 2 ? 0.2 : 0.4; height: 84; width: wrapper.width; y: 1 }
            Rectangle {
                x: 6; y: 4; width: 77; height: 77; color: "white"; smooth: true

                Image { source: imagePath; x: 1; y: 1 }
                Image { source: "images/gloss.png" }
            }
            Column {
                x: 92; width: wrapper.ListView.view.width - 95; y: 15; spacing: 2
                Text { text: title; color: "white"; width: parent.width; font.bold: true; elide: Text.ElideRight; style: Text.Raised; styleColor: "black" }
                Text { text: photoAuthor; width: parent.width; elide: Text.ElideLeft; color: "#cccccc"; style: Text.Raised; styleColor: "black" }
                Text { text: photoDate; width: parent.width; elide: Text.ElideRight; color: "#cccccc"; style: Text.Raised; styleColor: "black" }
            }
        }
    }
}
