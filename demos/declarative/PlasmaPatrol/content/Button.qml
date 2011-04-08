import QtQuick 2.0

//TODO: Add particles to this component too
Rectangle{
    id: container
    property alias text: txt.text
    signal clicked
    //color: "lightsteelblue"
    gradient: Gradient{
        GradientStop{ position: 0.0; color: "lightsteelblue" }
        GradientStop{ position: 1.0; color: "steelblue" }
    }
    height: 64
    radius: 16
    width: 128
    Text{
        id: txt
        anchors.centerIn: parent
        font.pixelSize: 24
        color: "white"
    }
    MouseArea{
        anchors.fill: parent
        onClicked: container.clicked()
    }
}
