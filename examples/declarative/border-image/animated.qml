import Qt 4.6

Rect {
    id: Page
    color: "white"
    width: 1030; height: 540

    BorderImage {
        x: 20; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "colors.png"; margin: 30
    }
    BorderImage {
        x: 270; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "colors.png"; margin: 30
        horizontalMode: "Repeat"; verticalMode: "Repeat"
    }
    BorderImage {
        x: 520; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "colors.png"; margin: 30
        horizontalMode: "Stretch"; verticalMode: "Repeat"
    }
    BorderImage {
        x: 770; y: 20; minWidth: 120; maxWidth: 240
        minHeight: 120; maxHeight: 240
        source: "colors.png"; margin: 30
        horizontalMode: "Round"; verticalMode: "Round"
    }
    BorderImage {
        x: 20; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200
        source: "bw.png"; margin: 10
    }
    BorderImage {
        x: 270; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200
        source: "bw.png"; margin: 10
        horizontalMode: "Repeat"; verticalMode: "Repeat"
    }
    BorderImage {
        x: 520; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200
        source: "bw.png"; margin: 10
        horizontalMode: "Stretch"; verticalMode: "Repeat"
    }
    BorderImage {
        x: 770; y: 280; minWidth: 60; maxWidth: 200
        minHeight: 40; maxHeight: 200
        source: "bw.png"; margin: 10
        horizontalMode: "Round"; verticalMode: "Round"
    }
}
