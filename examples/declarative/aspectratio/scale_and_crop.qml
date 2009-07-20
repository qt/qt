// Here, we implement "Scale and Crop" behaviour.
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
        scale: Math.max(parent.width/width,parent.height/height)
    }
}
