// Demonstrates opening new WebViews from HTML
//
// Note that to open windows from JavaScript, you will need to
// allow it on WebView with settings.javascriptCanOpenWindows: true

import Qt 4.6

Row {
    id: pages
    height: 200; width: 500

    Component {
        id: webViewPage
        Rectangle {
            width: webView.width
            height: webView.height
            border.color: "gray"

            WebView {
                id: webView
                newWindowComponent: webViewPage
                newWindowParent: pages
                url: "newwindows.html"
            }
        }
    }

    Loader { sourceComponent: webViewPage }
}
