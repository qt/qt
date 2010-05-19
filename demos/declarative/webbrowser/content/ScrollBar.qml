import Qt 4.7

Item {
    id: container

    property variant scrollArea
    property variant orientation: Qt.Vertical

    opacity: 0

    function position()
    {
        var ny = 0;
        if (container.orientation == Qt.Vertical)
            ny = scrollArea.visibleArea.yPosition * container.height;
        else
            ny = scrollArea.visibleArea.xPosition * container.width;
        if (ny > 2) return ny; else return 2;
    }

    function size()
    {
        var nh, ny;

        if (container.orientation == Qt.Vertical)
            nh = scrollArea.visibleArea.heightRatio * container.height;
        else
            nh = scrollArea.visibleArea.widthRatio * container.width;

        if (container.orientation == Qt.Vertical)
            ny = scrollArea.visibleArea.yPosition * container.height;
        else
            ny = scrollArea.visibleArea.xPosition * container.width;

        if (ny > 3) {
            var t;
            if (container.orientation == Qt.Vertical)
                t = Math.ceil(container.height - 3 - ny);
            else
                t = Math.ceil(container.width - 3 - ny);
            if (nh > t) return t; else return nh;
        } else return nh + ny;
    }

    Rectangle { anchors.fill: parent; color: "Black"; opacity: 0.5 }

    BorderImage {
        source: "pics/scrollbar.png"
        border { left: 1; right: 1; top: 1; bottom: 1 }
        x: container.orientation == Qt.Vertical ? 2 : position()
        width: container.orientation == Qt.Vertical ? container.width - 4 : size()
        y: container.orientation == Qt.Vertical ? position() : 2
        height: container.orientation == Qt.Vertical ? size() : container.height - 4
    }

    states: State {
        name: "visible"
        when: container.orientation == Qt.Vertical ? scrollArea.movingVertically : scrollArea.movingHorizontally
        PropertyChanges { target: container; opacity: 1.0 }
    }

    transitions: Transition {
        from: "visible"; to: ""
        NumberAnimation { properties: "opacity"; duration: 600 }
    }
}
