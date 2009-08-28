import Qt 4.6

// The WebView size is determined by the width, height,
// preferredWidth, and preferredHeight properties.
Rectangle {
    id: Rect
    color: "white"
    width: 200
    height: Layout.height
    Column {
        id: Layout
        spacing: 2
        WebView {
            html: "No width defined."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: Rect.width
            html: "The width is full."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: Rect.width/2
            html: "The width is half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            preferredWidth: Rect.width/2
            html: "The preferredWidth is half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            preferredWidth: Rect.width/2
            html: "The_preferredWidth_is_half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: Rect.width/2
            html: "The_width_is_half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
    }
}
