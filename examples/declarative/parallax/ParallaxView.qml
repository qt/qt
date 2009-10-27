import Qt 4.6

Item {
    property alias background: background.source

    Image {
        id: background
        fillMode: Image.TileHorizontally
        x: -list.viewportX / 2
        width: Math.max(list.viewportWidth, parent.width)
    }

    default property alias content: visualModel.children
    ListView {
        id: list
        orientation: "Horizontal"
        overShoot: false
        anchors.fill: parent
        model: VisualItemModel { id: visualModel }

        highlight: Item { height: 1; width: 1}
        preferredHighlightBegin: 0
        preferredHighlightEnd: 0
        highlightRangeMode: "StrictlyEnforceRange"
    }
}
