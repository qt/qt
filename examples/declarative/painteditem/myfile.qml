import QtQuick 2.0
import MyModule 1.0

Rectangle {
    width: 480
    height: 480
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#00249a" }
        GradientStop { position: 0.7; color: "#ffd94f" }
        GradientStop { position: 1.0; color: "#ffa322" }
    }
    MyPaintItem {
        anchors.fill: parent
        anchors.margins: 10
        smooth: true
    }
}
