import Qt 4.6

Item{
    property bool created: false
    property string image
    width: imageItem.width
    height: imageItem.height
    z: 2
    Image{
        id: imageItem
        source: image; 
    }
}
