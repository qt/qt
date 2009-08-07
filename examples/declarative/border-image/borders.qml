import Qt 4.6

Rect {
    id: Page
    color: "white"
    width: 520; height: 280

    Image {
        x: 20; y: 20; width: 230; height: 240
        smooth: true
        source: "colors-stretch.sci"
    }
    Image {
        x: 270; y: 20; width: 230; height: 240
        smooth: true
        source: "colors-round.sci"
    }
}
