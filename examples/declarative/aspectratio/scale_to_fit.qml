import Qt 4.7

// Here, we implement "Scale to Fit" behaviour "manually", rather
// than using the preserveAspect property.
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
        scale: Math.min(parent.width/width,parent.height/height)
    }
}
