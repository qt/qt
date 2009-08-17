import Qt 4.6

Item {
    property real value : 0

    width: 210; height: 210

    Image { id: Background; source: "background.png" }

    Image {
        x: 93
        y: 35
        source: "needle_shadow.png"
        transform: Rotation {
            origin.x: 11; origin.y: 67
            angle: NeedleRotation.angle
        }
    }
    Image {
        id: Needle
        x: 95; y: 33
        smooth: true
        source: "needle.png"
        transform: Rotation {
            id: NeedleRotation
            origin.x: 7; origin.y: 65
            angle: -130
            angle: Follow {
                spring: 1.4
                damping: .15
                source: Math.min(Math.max(-130, value*2.2 - 130), 133)
            }
        }
    }
    Image { x: 21; y: 18; source: "overlay.png" }
}
