import Qt 4.6

Rectangle {
    width: 400
    height: 400

    function setLoaderSource() {
        myLoader.source = "GreenRect.qml"
    }

    Loader {
        id: myLoader
    }
}
