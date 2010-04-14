import Qt 4.7

Item {
    id: scrollBar

    // The properties that define the scrollbar's state.
    // position and pageSize are in the range 0.0 - 1.0.  They are relative to the
    // height of the page, i.e. a pageSize of 0.5 means that you can see 50%
    // of the height of the view.
    // orientation can be either 'Vertical' or 'Horizontal'
    property real position
    property real pageSize
    property variant orientation : "Vertical"

    // A light, semi-transparent background
    Rectangle {
        id: background
        anchors.fill: parent
        radius: orientation == 'Vertical' ? (width/2 - 1) : (height/2 - 1)
        color: "white"
        opacity: 0.3
    }
    // Size the bar to the required size, depending upon the orientation.
    Rectangle {
        x: orientation == 'Vertical' ? 1 : (scrollBar.position * (scrollBar.width-2) + 1)
        y: orientation == 'Vertical' ? (scrollBar.position * (scrollBar.height-2) + 1) : 1
        width: orientation == 'Vertical' ? (parent.width-2) : (scrollBar.pageSize * (scrollBar.width-2))
        height: orientation == 'Vertical' ? (scrollBar.pageSize * (scrollBar.height-2)) : (parent.height-2)
        radius: orientation == 'Vertical' ? (width/2 - 1) : (height/2 - 1)
        color: "black"
        opacity: 0.7
    }
}
