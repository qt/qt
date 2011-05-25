import Qt 4.7
import Qt.labs.shaders 1.0

Item {
    id: main
    width: 360
    height: 640

    property bool liveShadows: true
    property real r: 0

    NumberAnimation on r {
        loops: Animation.Infinite
        from: 0
        to: 360
        duration: 3000
    }

    Image {
        id: background
        width: main.width
        height: main.height
        fillMode: Image.Tile
        source: "bg.jpg"
    }

    GaussianDropShadow {
        x: image1.x + 50
        y: image1.y + 50
        width: image1.width
        height: image1.height
        shadowColor: "#88000000"
        source: ShaderEffectSource { sourceItem: image1; hideSource: false; sourceRect: Qt.rect(-10, -10, image1.width + 20, image1.height + 20) }
        radius: 12.0
        deviation: 12
        rotation: r
    }

    Image {
        id: image1
        anchors.fill: parent
        source: "drop_shadow_small.png"
        smooth: true
        rotation: r
    }

    GaussianDropShadow {
        x: image2.x + 50
        y: image2.y + 50
        width: image2.width
        height: image2.height
        shadowColor: "#88000000"
        source: ShaderEffectSource { sourceItem: image2; hideSource: false; sourceRect: Qt.rect(-10, -10, image2.width + 20, image2.height + 20) }
        radius: 12.0
        deviation: 12
        rotation: -r
    }

    Image {
        id: image2
        anchors.fill: parent
        source: "drop_shadow_small.png"
        smooth: true
        rotation: -r
    }
}
