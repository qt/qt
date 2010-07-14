import Qt 4.7

Rectangle {
    width: 300; height: 200;

    property bool leftToRight: false

    Flow {
        objectName: "flow"
        flow: leftToRight ? Flow.LeftToRight : Flow.TopToBottom
        spacing: 20
        anchors.horizontalCenter: parent.horizontalCenter
        Rectangle { color: "red"; width: 100; height: 50 }
        Rectangle { color: "blue"; width: 100; height: 50 }
    }
}