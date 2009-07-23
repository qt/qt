import Qt 4.6

// The WebView size is determined by the width, height,
// idealWidth, and idealHeight properties.
Rect {
    id: Rect
    color: "white"
    width: 200
    height: Layout.height
    VerticalLayout {
        id: Layout
        spacing: 2
        WebView {
            html: "No width defined."
            Rect {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: Rect.width
            html: "The width is full."
            Rect {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: Rect.width/2
            html: "The width is half."
            Rect {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            idealWidth: Rect.width/2
            html: "The idealWidth is half."
            Rect {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            idealWidth: Rect.width/2
            html: "The_idealWidth_is_half."
            Rect {
                color: "#10000000"
                anchors.fill: parent
            }
        }
        WebView {
            width: Rect.width/2
            html: "The_width_is_half."
            Rect {
                color: "#10000000"
                anchors.fill: parent
            }
        }
    }
}
