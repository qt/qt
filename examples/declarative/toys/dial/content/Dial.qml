import Qt 4.7

Item {
    id: root
    property real value : 0

    width: 210; height: 210

    Image { source: "background.png" }

    Image {
        x: 93
        y: 35
        source: "needle_shadow.png"
        transform: Rotation {
            origin.x: 11; origin.y: 67
            angle: needleRotation.angle
        }
    }
    Image {
        id: needle
        x: 95; y: 33
        smooth: true
        source: "needle.png"
        transform: Rotation {
            id: needleRotation
            origin.x: 7; origin.y: 65
            angle: -130
            SpringFollow on angle {
                spring: 1.4
                damping: .15
                to: Math.min(Math.max(-130, root.value*2.6 - 130), 133)
            }
        }
    }
    Image { x: 21; y: 18; source: "overlay.png" }
}
