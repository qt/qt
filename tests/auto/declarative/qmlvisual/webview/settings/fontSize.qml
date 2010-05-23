import Qt 4.7
import org.webkit 1.0

Grid {
    columns: 3
    Rectangle {
        Text { color: "green"; text: "Normal" }
        border.color: "black"
        width: 200
        height: 200
        WebView {
            anchors.fill: parent
            url: "test.html"
        }
    }
    Rectangle {
        Text { color: "green"; text: "Big" }
        border.color: "black"
        width: 200
        height: 200
        WebView {
            anchors.fill: parent
            url: "test.html"
            settings.minimumFontSize: 20
        }
    }
    Rectangle {
        Text { color: "green"; text: "Big (logical)" }
        border.color: "black"
        width: 200
        height: 200
        WebView {
            anchors.fill: parent
            url: "test.html"
            settings.minimumLogicalFontSize: 20
        }
    }
    Rectangle {
        Text { color: "green"; text: "Bigger" }
        border.color: "black"
        width: 200
        height: 200
        WebView {
            anchors.fill: parent
            url: "test.html"
            settings.minimumFontSize: 30
        }
    }
    Rectangle {
        Text { color: "green"; text: "Small (except fixed)" }
        border.color: "black"
        width: 200
        height: 200
        WebView {
            anchors.fill: parent
            url: "test.html"
            settings.defaultFontSize: 8
        }
    }
    Rectangle {
        Text { color: "green"; text: "Small fixed" }
        border.color: "black"
        width: 200
        height: 200
        WebView {
            anchors.fill: parent
            url: "test.html"
            settings.defaultFixedFontSize: 8
        }
    }
}
