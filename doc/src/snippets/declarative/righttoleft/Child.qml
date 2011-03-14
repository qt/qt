import QtQuick 1.0

Rectangle {
    width: 50; height: 50
    color: "black"
    Text {
        color: "white"
        text: String.fromCharCode(65 + Math.floor(26*Math.random()))
        anchors.centerIn: parent
    }
}