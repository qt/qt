// This example demonstrates placing items in a view using
// a VisualItemModel

import Qt 4.6

Rectangle {
    color: "lightgray"
    width: 240
    height: 320

    VisualItemModel {
        id: itemModel
        Rectangle {
            height: view.height; width: view.width; color: "#FFFEF0"
            Text { text: "Page 1"; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            height: view.height; width: view.width; color: "#F0FFF7"
            Text { text: "Page 2"; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            height: view.height; width: view.width; color: "#F4F0FF"
            Text { text: "Page 3"; font.bold: true; anchors.centerIn: parent }
        }
    }

    ListView {
        id: view
        anchors.fill: parent; anchors.bottomMargin: 30
        model: itemModel
        preferredHighlightBegin: 0; preferredHighlightEnd: 0
        highlightRangeMode: "StrictlyEnforceRange"
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem; flickDeceleration: 2000
    }

    Rectangle {
        color: "gray"
        anchors.top: view.bottom
        anchors.bottom: parent.bottom
        height: 30
        width: 240

        Row {
            anchors.centerIn: parent
            spacing: 20
            Repeater {
                model: itemModel.count
                Rectangle {
                    width: 5; height: 5
                    radius: 3
                    MouseArea { width: 20; height: 20; anchors.centerIn: parent; onClicked: view.currentIndex = index }
                    color: view.currentIndex == index ? "blue" : "white"
                }
            }
        }
    }
}
