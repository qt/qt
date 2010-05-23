import Qt 4.7
import org.webkit 1.0

// The WebView size is determined by the width, height,
// preferredWidth, and preferredHeight properties.
Rectangle {
    id: rect
    color: "white"
    width: 200
    height: layout.height
    Column {
        id: layout
        spacing: 2
        WebView {
            html: "No width defined."
            Rectangle {                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: rect.width
            html: "The width is full."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: rect.width/2
            html: "The width is half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            preferredWidth: rect.width/2
            html: "The preferredWidth is half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            preferredWidth: rect.width/2
            html: "The_preferredWidth_is_half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: rect.width/2
            html: "The_width_is_half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
    }
}
