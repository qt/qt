import Qt 4.7

// Here, we implement a variant of "Scale and Crop" behaviour, where we
// crop the sides if necessary to fully fit vertically, but not the reverse.
//
Rectangle {
    // default size: whole image, unscaled
    width: face.width
    height: face.height
    color: "gray"
    clip: true

    Image {
        id: face
        smooth: true
        anchors.centerIn: parent
        source: "pics/face.png"
        x: (parent.width-width*scale)/2
        y: (parent.height-height*scale)/2
        scale: parent.height/height
    }
}
