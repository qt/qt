import Qt 4.6

// Here, we implement "Scale to Fit" behaviour "manually", rather
// than using the preserveAspect property.
//
Rect {
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
        scale: Math.min(parent.width/width,parent.height/height)
    }
}
