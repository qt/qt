import QtQuick 1.0
import "content"

Rectangle {
    id: page
    color: "white"
    width: 1030; height: 540

    MyBorderImage {
        x: 20; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "content/colors.png"; margin: 30; antialiased: true
    }
    MyBorderImage {
        x: 270; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240; antialiased: true
        source: "content/colors.png"; margin: 30
        horizontalMode: BorderImage.Repeat; verticalMode: BorderImage.Repeat
    }
    MyBorderImage {
        x: 520; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240; antialiased: true
        source: "content/colors.png"; margin: 30
        horizontalMode: BorderImage.Stretch; verticalMode: BorderImage.Repeat
    }
    MyBorderImage {
        x: 770; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240; antialiased: true
        source: "content/colors.png"; margin: 30
        horizontalMode: BorderImage.Round; verticalMode: BorderImage.Round
    }
    MyBorderImage {
        x: 20; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200; antialiased: true
        source: "content/bw.png"; margin: 10
    }
    MyBorderImage {
        x: 270; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200; antialiased: true
        source: "content/bw.png"; margin: 10
        horizontalMode: BorderImage.Repeat; verticalMode: BorderImage.Repeat
    }
    MyBorderImage {
        x: 520; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200; antialiased: true
        source: "content/bw.png"; margin: 10
        horizontalMode: BorderImage.Stretch; verticalMode: BorderImage.Repeat
    }
    MyBorderImage {
        x: 770; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200; antialiased: true
        source: "content/bw.png"; margin: 10
        horizontalMode: BorderImage.Round; verticalMode: BorderImage.Round
    }
}
