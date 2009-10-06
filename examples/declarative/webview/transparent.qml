import Qt 4.6

// The WebView background is transparent
// if the HTML does not specify a background
Rectangle {
    color: "green"
    width: web.width
    height: web.height
    WebView {
        id: web
        html: "Hello <b>World!</b>"
    }
}
