// This example demonstrates placing items in a view using
// a VisualItemModel

import Qt 4.6

Rectangle {
    color: "lightgray"
    width: 240
    height: 320

    VisualItemModel {
        id: ItemModel
        Rectangle {
            height: View.height; width: View.width; color: "#FFFEF0"
            Text { text: "Page 1"; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            height: View.height; width: View.width; color: "#F0FFF7"
            Text { text: "Page 2"; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            height: View.height; width: View.width; color: "#F4F0FF"
            Text { text: "Page 3"; font.bold: true; anchors.centerIn: parent }
        }
    }

    ListView {
        id: View
        anchors.fill: parent
        anchors.bottomMargin: 30
        model: ItemModel
        currentItemPositioning: "SnapAuto"
        orientation: "Horizontal"
    }

    Rectangle {
        color: "gray"
        anchors.top: View.bottom
        anchors.bottom: parent.bottom
        height: 30
        width: 240

        Row {
            anchors.centerIn: parent
            spacing: 20
            Repeater {
                model: ItemModel.count
                Rectangle {
                    width: 5; height: 5
                    radius: 3
                    MouseRegion { width: 20; height: 20; anchors.centerIn: parent; onClicked: View.currentIndex = index }
                    color: View.currentIndex == index ? "blue" : "white"
                }
            }
        }
    }
}
