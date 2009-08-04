import Qt 4.6

Item {
    id: MyRect
    property string color
    property string border : ""
    property int rotation
    property int radius
    property int borderWidth
    property bool smooth: false

    width: 80; height: 80
    Item {
        anchors.centerIn: parent; rotation: MyRect.rotation;
        Rect {
            anchors.centerIn: parent; width: 80; height: 80
            color: MyRect.color; border.color: MyRect.border; border.width: MyRect.border != "" ? 2 : 0
            radius: MyRect.radius; smooth: MyRect.smooth
        }
    }
}
