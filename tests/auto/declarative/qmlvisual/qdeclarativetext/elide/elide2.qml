import Qt 4.7

Rectangle {
    width: 500
    height: 100

    Text {
        width: NumberAnimation { from: 500; to: 0; loops: Animation.Infinite; duration: 5000 }
        elide: Text.ElideRight
        text: 'Here is some very long text that we should truncate when sizing window'
    }
}
