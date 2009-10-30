import Qt 4.6

Image {
    id: tree
    property bool created: false
    opacity: y+height > window.height/2 ? 1 : 0.25
    onCreatedChanged: if (created && y+height<=window.height/2) { tree.destroy() }
    scale: y+height > window.height/2 ? (y+height-250)*0.01 : 1
    transformOrigin: "Center"
    source: image; 
    z: y
}
