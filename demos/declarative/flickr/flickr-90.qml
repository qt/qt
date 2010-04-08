import Qt 4.7

Item {
    width: 480; height: 320

    Loader {
        y: 320; rotation: -90
        transformOrigin: Item.TopLeft
        source: "flickr.qml"
    }
}
