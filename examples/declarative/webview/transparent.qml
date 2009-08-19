import Qt 4.6

// The WebView background is transparent
// if the HTML does not specify a background
Rectangle {
    color: "green"
    width: Web.width
    height: Web.height
    WebView {
        id: Web
        html: "Hello <b>World!</b>"
    }
}
