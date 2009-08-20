import Qt 4.6

Rectangle {
    id: Page
    color: "white"
    width: 1030; height: 540

    MyBorderImage {
        x: 20; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "colors.png"; margin: 30
    }
    MyBorderImage {
        x: 270; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "colors.png"; margin: 30
        horizontalMode: "Repeat"; verticalMode: "Repeat"
    }
    MyBorderImage {
        x: 520; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "colors.png"; margin: 30
        horizontalMode: "Stretch"; verticalMode: "Repeat"
    }
    MyBorderImage {
        x: 770; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "colors.png"; margin: 30
        horizontalMode: "Round"; verticalMode: "Round"
    }
    MyBorderImage {
        x: 20; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200
        source: "bw.png"; margin: 10
    }
    MyBorderImage {
        x: 270; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200
        source: "bw.png"; margin: 10
        horizontalMode: "Repeat"; verticalMode: "Repeat"
    }
    MyBorderImage {
        x: 520; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200
        source: "bw.png"; margin: 10
        horizontalMode: "Stretch"; verticalMode: "Repeat"
    }
    MyBorderImage {
        x: 770; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200
        source: "bw.png"; margin: 10
        horizontalMode: "Round"; verticalMode: "Round"
    }
}
