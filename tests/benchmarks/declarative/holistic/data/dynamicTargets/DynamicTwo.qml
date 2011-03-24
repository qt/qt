import QtQuick 1.0

Item {
    id: root
    property int dynamicWidth: 100
    property int dynamicHeight: rect1.height + rect2.height

    Rectangle {
        id: rect1
        width: root.dynamicWidth + 20
        height: width + (5*3) - 8 + (width/9)
        color: "red"
        border.color: "black"
        border.width: 5
        radius: 10
    }

    Rectangle {
        id: rect2
        width: rect1.width - 50
        height: width + (5*4) - 6 + (width/3)
        color: "red"
        border.color: "black"
        border.width: 5
        radius: 10
    }
}
