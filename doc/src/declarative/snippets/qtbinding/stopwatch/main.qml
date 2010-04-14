//![0]
import Qt 4.7

Rectangle {
    width: 300
    height: 300

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (stopwatch.isRunning())
                stopwatch.stop()
            else
                stopwatch.start();
        }
    }
}
//![0]
