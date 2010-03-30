import Qt 4.7

// Here, we extend the "face_fit" example with animation to show how truly
// diverse and usage-specific behaviours are made possible by NOT putting a
// hard-coded aspect ratio feature into the Image primitive.
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
        SpringFollow on scale {
            to: Math.max(Math.min(face.parent.width/face.width*1.333,face.parent.height/face.height),
                         Math.min(face.parent.width/face.width,face.parent.height/face.height*1.333))
            spring: 1
            damping: 0.05
        }
    }
}
