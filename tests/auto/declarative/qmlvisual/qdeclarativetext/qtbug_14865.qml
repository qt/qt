import QtQuick 1.0

Rectangle {
    width: 200; height: 200

    Text {
        id: label
        objectName: "label"
        text: "Hello world!"
        width: 10
    }

    Timer {
        running: true; interval: 1000
        onTriggered: label.text = ""
    }
}
