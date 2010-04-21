// This example demonstrates placing items in a view using
// a VisualItemModel

import Qt 4.7

Rectangle {
    color: "lightgray"
    width: 240
    height: 320

    VisualItemModel {
        id: itemModel

        Rectangle {
            width: view.width; height: view.height
            color: "#FFFEF0"
            Text { text: "Page 1"; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            width: view.width; height: view.height
            color: "#F0FFF7"
            Text { text: "Page 2"; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            width: view.width; height: view.height
            color: "#F4F0FF"
            Text { text: "Page 3"; font.bold: true; anchors.centerIn: parent }
        }
    }

    ListView {
        id: view
        anchors { fill: parent; bottomMargin: 30 }
        model: itemModel
        preferredHighlightBegin: 0; preferredHighlightEnd: 0
        highlightRangeMode: "StrictlyEnforceRange"
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem; flickDeceleration: 2000
    }

    Rectangle {
        width: 240; height: 30
        anchors { top: view.bottom; bottom: parent.bottom }
        color: "gray"

        Row {
            anchors.centerIn: parent
            spacing: 20

            Repeater {
                model: itemModel.count

                Rectangle {
                    width: 5; height: 5
                    radius: 3
                    color: view.currentIndex == index ? "blue" : "white"

                    MouseArea { 
                        width: 20; height: 20
                        anchors.centerIn: parent
                        onClicked: view.currentIndex = index
                    }
                }
            }
        }
    }
}
