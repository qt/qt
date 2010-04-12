import Qt 4.7

// Here, we implement a hybrid of the "scale to fit" and "scale and crop"
// behaviours which will crop up to 25% from *one* dimension if necessary
// to fully scale the other. This is a realistic algorithm, for example
// when the edges of the image contain less vital information than the
// center - such as a face.
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
        scale: Math.max(Math.min(parent.width/width*1.333,parent.height/height),
                        Math.min(parent.width/width,parent.height/height*1.333))
    }
}
