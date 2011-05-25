import Qt 4.7
import Qt.labs.shaders 1.0

Item {
    id: gaussianDropShadow

    property color shadowColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
    property variant source: 0
    property real radius: 8
    property real deviation: Math.sqrt(-((radius+1) * (radius+1)) / (2 * Math.log(1.0 / 255.0)))
    property bool live: true

    GaussianBlur {
        id: blur
        anchors.fill: parent
        radius: gaussianDropShadow.radius
        deviation: gaussianDropShadow.deviation
        source: gaussianDropShadow.source
        live: gaussianDropShadow.live
    }

    ShaderEffectItem {
        id: shadow
        property color shadowColor: gaussianDropShadow.shadowColor
        property variant source: ShaderEffectSource { sourceItem: blur; hideSource: true }
        anchors.fill: parent

        fragmentShader:
            "
            varying mediump vec2 qt_TexCoord0;
            uniform sampler2D source;
            uniform lowp vec4 shadowColor;

            void main() {
                lowp vec4 sourceColor = texture2D(source, qt_TexCoord0);
                gl_FragColor = mix(vec4(0), shadowColor, sourceColor.a);
            }
            "
    }
}

