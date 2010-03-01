import Qt 4.6

Rectangle {
    width: 640
    height: 480
    // Create a flickable to view a large image.
    Flickable {
        id: view
        anchors.fill: parent
        Image {
            id: picture
            source: "pics/niagara_falls.jpg"
            asynchronous: true
        }
        contentWidth: picture.width
        contentHeight: picture.height
        // Only show the scrollbars when the view is moving.
        states: [
            State {
                name: "ShowBars"
                when: view.moving
                PropertyChanges { target: verticalScrollBar; opacity: 1 }
                PropertyChanges { target: horizontalScrollBar; opacity: 1 }
            }
        ]
        transitions: [
            Transition {
                from: "*"
                to: "*"
                NumberAnimation {
                    properties: "opacity"
                    duration: 400
                }
            }
        ]
    }
    // Attach scrollbars to the right and bottom edges of the view.
    ScrollBar {
        id: verticalScrollBar
        opacity: 0
        orientation: "Vertical"
        position: view.visibleArea.yPosition
        pageSize: view.visibleArea.heightRatio
        width: 12
        height: view.height-12
        anchors.right: view.right
    }
    ScrollBar {
        id: horizontalScrollBar
        opacity: 0
        orientation: "Horizontal"
        position: view.visibleArea.xPosition
        pageSize: view.visibleArea.widthRatio
        height: 12
        width: view.width-12
        anchors.bottom: view.bottom
    }
}
