import Qt 4.6

// The WebView size is determined by the width, height,
// idealWidth, and idealHeight properties.
Rectangle {
    id: Rect
    color: "white"
    width: 200
    height: Layout.height
    VerticalPositioner {
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
            idealWidth: Rect.width/2
            html: "The idealWidth is half."
            Rectangle {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            idealWidth: Rect.width/2
            html: "The_idealWidth_is_half."
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
