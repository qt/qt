import Qt 4.6

Item {
    id: page
    property int current: 0
    default property alias content: stack.children
    onCurrentChanged: setOpacities()
    Component.onCompleted: setOpacities()
    function setOpacities()
    {
        for (var i=0; i<stack.children.length; ++i) {
            stack.children[i].opacity = i==current ? 1 : 0
        }
    }
    Row {
        id: header
        Repeater {
            delegate:
                Rectangle {
                    width: page.width / stack.children.length
                    height: 15
                    color: page.current == index ? "grey" : "lightGrey"
                    Text {
                        anchors.fill: parent
                        text: stack.children[index].title
                        elide: Text.ElideRight
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: page.current = index
                    }
                }
            model: stack.children.length
        }
    }
    Item {
        id: stack
        anchors.top: header.bottom
        anchors.bottom: page.bottom
        width: page.width
    }
}
