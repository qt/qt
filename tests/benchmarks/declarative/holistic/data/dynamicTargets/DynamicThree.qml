import QtQuick 1.0

Item {
    id: root
    property int dynamicWidth: 10
    property int widthSignaledProperty: 20

    Rectangle {
        width: 100
        height: root.dynamicWidth + (5*3) - 8 + (root.dynamicWidth/10)
        color: "red"
        border.color: "black"
        border.width: 5
        radius: 10
    }

    onDynamicWidthChanged: {
        widthSignaledProperty = dynamicWidth + (20/4) + 7 - 1;
    }
}
