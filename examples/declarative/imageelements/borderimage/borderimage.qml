import Qt 4.7
import "content"

Rectangle {
    id: page
    width: 1030; height: 540

    Grid {
        anchors.centerIn: parent; spacing: 20

        MyBorderImage {
            minWidth: 120; maxWidth: 240; minHeight: 120; maxHeight: 240
            source: "content/colors.png"; margin: 30
        }

        MyBorderImage {
            minWidth: 120; maxWidth: 240; minHeight: 120; maxHeight: 240
            source: "content/colors.png"; margin: 30
            horizontalMode: BorderImage.Repeat; verticalMode: BorderImage.Repeat
        }

        MyBorderImage {
            minWidth: 120; maxWidth: 240; minHeight: 120; maxHeight: 240
            source: "content/colors.png"; margin: 30
            horizontalMode: BorderImage.Stretch; verticalMode: BorderImage.Repeat
        }

        MyBorderImage {
            minWidth: 120; maxWidth: 240; minHeight: 120; maxHeight: 240
            source: "content/colors.png"; margin: 30
            horizontalMode: BorderImage.Round; verticalMode: BorderImage.Round
        }

        MyBorderImage {
            minWidth: 60; maxWidth: 200; minHeight: 40; maxHeight: 200
            source: "content/bw.png"; margin: 10
        }

        MyBorderImage {
            minWidth: 60; maxWidth: 200; minHeight: 40; maxHeight: 200
            source: "content/bw.png"; margin: 10
            horizontalMode: BorderImage.Repeat; verticalMode: BorderImage.Repeat
        }

        MyBorderImage {
            minWidth: 60; maxWidth: 200; minHeight: 40; maxHeight: 200
            source: "content/bw.png"; margin: 10
            horizontalMode: BorderImage.Stretch; verticalMode: BorderImage.Repeat
        }

        MyBorderImage {
            minWidth: 60; maxWidth: 200; minHeight: 40; maxHeight: 200
            source: "content/bw.png"; margin: 10
            horizontalMode: BorderImage.Round; verticalMode: BorderImage.Round
        }
    }
}
