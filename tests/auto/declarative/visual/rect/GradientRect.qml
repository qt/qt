import Qt 4.6

Item {
    id: rect
    property string color
    property string border : ""
    property int rotation
    property int radius
    property int borderWidth
    property bool smooth: false

    width: 80; height: 80
    Item {
        anchors.centerIn: parent; rotation: rect.rotation;
        Rectangle {
            anchors.centerIn: parent; width: 80; height: 80
            border.color: rect.border; border.width: rect.border != "" ? 2 : 0
            radius: rect.radius; smooth: rect.smooth
            gradient: Gradient {
                GradientStop { position: 0.0; color: rect.color }
                GradientStop { position: 1.0; color: "white" }
            }
        }
    }
}
