import Qt 4.7

Image {
    id: tree
    property bool created: false
    property double scaleFactor: Math.max((y+height-250)*0.01, 0.3)
    property double scaledBottom: y + (height+height*scaleFactor)/2 
    property bool onLand: scaledBottom > window.height/2
    property string image //Needed for compatibility with GenericItem
    opacity: onLand ? 1 : 0.25
    onCreatedChanged: if (created && !onLand) { tree.destroy() } else { z = scaledBottom }
    scale: scaleFactor
    transformOrigin: "Center"
    source: image; smooth: true
    onYChanged: z = scaledBottom
}
