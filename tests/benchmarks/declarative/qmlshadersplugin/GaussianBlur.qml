import Qt 4.7
import Qt.labs.shaders 1.0

Item {
    id: gaussianBlur
    property variant source: 0
    property real radius: 8;
    property real deviation: Math.sqrt(-((radius+1) * (radius+1)) / (2 * Math.log(1.0 / 255.0)))
    property bool live: true

    ShaderEffectItem {
        id: cache
        anchors.fill: parent
        visible: !gaussianBlur.live
        property variant source: ShaderEffectSource { sourceItem: verticalBlur; live: false; hideSource: true }
    }

    GaussianDirectionalBlur {
        id: verticalBlur
        anchors.fill: parent

        deltaX: 0.0
        deltaY: 1.0/parent.height

        source: ShaderEffectSource { sourceItem: horizontalBlur; hideSource: true }
        deviation: gaussianBlur.deviation
        radius: gaussianBlur.radius
    }

    GaussianDirectionalBlur {
        id: horizontalBlur
        anchors.fill: parent
        blending: false

        deltaX: 1.0/parent.width
        deltaY: 0.0

        source: gaussianBlur.source
        deviation: gaussianBlur.deviation
        radius: gaussianBlur.radius
    }

}
