Item {
    property real value : 0

    width: 210; height: 210

    Image { id: Background; source: "background.svg" }

    Image {
        source: "needle_shadow.svg"
        transform: Rotation {
            originX: 104; originY: 102
            angle: NeedleRotation.angle
        }
    }
    Image {
        id: Needle
        source: "needle.svg"
        transform: Rotation {
            id: NeedleRotation
            originX: 102; originY: 98
            angle: -130
            angle: Follow {
                spring: 1.4
                damping: .15
                source: Math.min(Math.max(-130, value*2.2 - 130), 133)
            }
        }
    }
    Image { source: "overlay.svg" }
}
