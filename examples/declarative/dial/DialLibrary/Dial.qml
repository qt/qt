Item {
    property real value : 0

    width: 210; height: 210

    Image { id: Background; source: "background.svg" }
    Item {
        x: 104; y: 102
        rotation: Needle.rotation
        Image {
            source: "needle_shadow.svg"
            x: -104; y: -102
        }
    }
    Item {
        id: Needle
        x: 102; y: 98
        rotation: -130
        rotation: Follow {
            spring: 1.4
            damping: .15
            source: Math.min(Math.max(-130, value*2.2 - 130), 133)
        }
        Image {
            source: "needle.svg"
            x: -102; y: -98
        }
    }
    Image { source: "overlay.svg" }
}
