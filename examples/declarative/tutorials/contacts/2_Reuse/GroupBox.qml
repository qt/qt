import Qt 4.6

FocusScope {
    id: groupBox
    width: Math.max(270, subItem.width+40)
    height: Math.max(70, subItem.height+40)

    property var contents
    property var label

    Rectangle {
        id: wrapper
        x: 5
        y: 10
        radius: 10
        width: groupBox.width-20
        height: groupBox.height-20
        color: "white"
        border.color: "black"
        Loader {
            id: subItem
            source: groupBox.contents
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            width: item.width
            height: item.height
        }
    }
    Rectangle {
        x: 20
        y: 0
        height: text.height
        width: text.width+10
        color: "white"
        Text {
            x: 5
            id: text
            text: label
            font.bold: true
        }
    }
    Rectangle {
        color: "black"
        anchors.fill: parent
        opacity: parent.focus ? 0 : 0.3
        MouseRegion {
            anchors.fill: parent
            onClicked: { parent.parent.focus=true }
        }
        opacity: Behavior {
            NumberAnimation {
                property: "opacity"
                duration: 250
            }
        }
    }
}
