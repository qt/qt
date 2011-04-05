import QtQuick 1.0

Item {
    id: root
    property int dynamicWidth: 10

    Rectangle {
        width: 100
        height: root.dynamicWidth + (5*3) - 8 + (root.dynamicWidth/10)
        color: "red"
        border.color: "black"
        border.width: 5
        radius: 10
    }
}
