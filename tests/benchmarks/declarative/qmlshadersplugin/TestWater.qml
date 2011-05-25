import Qt 4.7
import Qt.labs.shaders 1.0

Item {
    width: 360
    height: 640

    Image {
        id: image
        width: parent.width
        height: parent.height * 0.65
        source: "sky.jpg"
        smooth: true
    }
    Water {
        sourceItem: image
        intensity: 5
        height: parent.height - image.height
    }
}
