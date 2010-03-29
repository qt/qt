import Qt 4.6
import "MyWidgets" 1.0

Rectangle {
    id: window
    width: 640; height: 480; color: palette.window

    property int margin: 30

    SystemPalette { id: palette }

    MyPushButton {
        id: button1; x: margin; y: margin; text: "Right"; onClicked: window.state = "right"
        transformOriginPoint: Qt.point(width / 2, height / 2)
    }

    MyPushButton {
        id: button2; x: margin; y: margin + 30; text: "Bottom"; onClicked: window.state = "bottom"
        transformOriginPoint: Qt.point(width / 2, height / 2)
    }

    MyPushButton {
        id: button3; x: margin; y: margin + 60; text: "Quit"; onClicked: Qt.quit()
        transformOriginPoint: Qt.point(width / 2, height / 2)
    }

    states: [
    State {
        name: "right"
        PropertyChanges { target: button1; x: window.width - width - margin; text: "Left"; onClicked: window.state = "" }
        PropertyChanges { target: button2; x: window.width - width - margin }
        PropertyChanges { target: button3; x: window.width - width - margin }
        PropertyChanges { target: window; color: Qt.darker(palette.window) }
    },
    State {
        name: "bottom"
        PropertyChanges { target: button1; y: window.height - height - margin; rotation: 180 }
        PropertyChanges {
            target: button2; x: button1.x + button1.width + 10; y: window.height - height - margin;  rotation: 180
            text: "Top"; onClicked: window.state = ""
        }
        PropertyChanges { target: button3; x: button2.x + button2.width + 10; y: window.height - height - margin;  rotation: 180 }
        PropertyChanges { target: window; color: Qt.lighter(palette.window) }
    }
    ]

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { properties: "x,y,rotation"; duration: 600; easing.type: "OutQuad" }
            ColorAnimation { target: window; duration: 600 }
        }
    }
}
