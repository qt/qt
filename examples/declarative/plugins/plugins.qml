import Qt 4.6
import com.nokia.TimeExample 1.0

Item {

    Time { id: time }

    width: 200
    height: 100

    Rectangle { 
        anchors.fill: parent
        color: "blue"
    }

    Text {
        text: time.hour + ":" + (time.minute < 10 ? "0" : "") + time.minute
        anchors.centerIn: parent
        color: "white"
        font.pixelSize: 42
        font.bold: true
    }
}
