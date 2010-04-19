import Qt 4.7
import org.webkit 1.0

WebView {
    id: webView
    width: 120
    height: 150
    url: "alerts.html"

    onAlert: popup.show(message)

    Rectangle {
        id: popup

        color: "red"
        border.color: "black"; border.width: 2
        radius: 4

        y: parent.height // off "screen"
        anchors.horizontalCenter: parent.horizontalCenter
        width: label.width+5
        height: label.height+5

        opacity: 0

        function show(t) {
            label.text = t
            popup.state = "visible"
            timer.start()
        }
        states: State {
            name: "visible"
            PropertyChanges { target: popup; opacity: 1 }
            PropertyChanges { target: popup; y: (webView.height-popup.height)/2 }
        }

        transitions: [
            Transition { from: ""; PropertyAnimation { properties: "opacity,y"; duration: 65 } },
            Transition { from: "visible"; PropertyAnimation { properties: "opacity,y"; duration: 500 } }
        ]

        Timer {
            id: timer
            interval: 1000
            onTriggered: popup.state = ""
        }

        Text {
            id: label
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 20
            width: webView.width*0.75
            wrapMode: Text.WordWrap
            horizontalAlignment: "AlignHCenter"
        }
    }
}
