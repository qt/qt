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
        Rectangle {
            anchors.centerIn: parent; width: 80; height: 80
            border.color: MyRect.border; border.width: MyRect.border != "" ? 2 : 0
            radius: MyRect.radius; smooth: MyRect.smooth
            gradient: Gradient {
                GradientStop { position: 0.0; color: MyRect.color }
                GradientStop { position: 1.0; color: "white" }
            }
        }
    }
}
