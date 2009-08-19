import Qt 4.6

// Here, we extend the "face_fit" example with animation to show how truly
// diverse and usage-specific behaviours are made possible by NOT putting a
// hard-coded aspect ratio feature into the Image primitive.
//
Rectangle {
    // default size: whole image, unscaled
    width: Image.width
    height: Image.height
    color: "gray"
    clip: true

    Image {
        id: Image
        source: "pics/face.png"
        x: (parent.width-width*scale)/2
        y: (parent.height-height*scale)/2
        scale: Follow {
            source: Math.max(Math.min(Image.parent.width/Image.width*1.333,Image.parent.height/Image.height),
                        Math.min(Image.parent.width/Image.width,Image.parent.height/Image.height*1.333))
            spring: 1
            damping: 0.05
        }
    }
}
