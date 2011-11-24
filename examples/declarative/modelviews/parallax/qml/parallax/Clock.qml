import QtQuick 1.1

Text {
    Timer {
        triggeredOnStart: true
        onTriggered: parent.text = Qt.formatTime(new Date())
        interval: 1000; running: visible; repeat: true;
    }

    font.pixelSize: 32
    font.family: "sans-serif"
    font.bold: true
    color: "white"
}
