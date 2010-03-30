import Qt 4.6

Item {
    id: tabWidget
    property int current: 0
    default property alias content: stack.children

    onCurrentChanged: setOpacities()
    Component.onCompleted: setOpacities()

    function setOpacities()
    {
        for (var i = 0; i < stack.children.length; ++i) {
            stack.children[i].opacity = i == current ? 1 : 0
        }
    }

    Row {
        id: header
        Repeater {
            delegate:
            Rectangle {
                width: tabWidget.width / stack.children.length; height: 36
                Rectangle {
                    color: "#acb2c2"; width: parent.width; height: 1
                    anchors { bottom: parent.bottom; bottomMargin: 1 }
                }
                BorderImage {
                    source: "tab.png"; visible: tabWidget.current == index; border.left: 7; border.right: 7
                    anchors { fill: parent; leftMargin: 2; topMargin: 5; rightMargin: 1 }
                }
                Text {
                    horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter
                    anchors.fill: parent; text: stack.children[index].title
                    elide: Text.ElideRight; font.bold: tabWidget.current == index
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: tabWidget.current = index
                }
            }
            model: stack.children.length
        }
    }

    Item {
        id: stack
        anchors.top: header.bottom; anchors.bottom: tabWidget.bottom; width: tabWidget.width
    }
}
