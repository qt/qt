Item {
    width: 210
    height: 210
    properties: Property {
        name: "value"
        type: "real"
        value: 0
    }
    Image {
        id: Background
        src: "background.svg"
    }
    Item {
        x: 104
        y: 102
        rotation: Needle.rotation
        Image {
            src: "needle_shadow.svg"
            x: -104
            y: -102
        }
    }
    Item {
        id: Needle
        x: 102
        y: 98
        rotation: -130
        rotation: Follow {
            spring: 1.4
            damping: .15
            source: Math.min(Math.max(-130, value*2.2 - 130), 133)
        }
        Image {
            src: "needle.svg"
            x: -102
            y: -98
        }
    }
    Image {
        src: "overlay.svg"
    }
}
