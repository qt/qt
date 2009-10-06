// Demonstrates opening new WebViews from HTML
//
// Note that to open windows from JavaScript, you will need to
// allow it on WebView with settings.javascriptCanOpenWindows: true

import Qt 4.6

Row {
    id: pages
    height: 200
    resources: [
        Component {
            id: webViewPage
                Rectangle {
                    width: webView.width
                    height: webView.height
                    WebView {
                        id: webView
                        newWindowComponent: webViewPage
                        newWindowParent: pages
                        url: "newwindows.html"
                    }
                }
        }
    ]
    width: 500
    Loader { sourceComponent: webViewPage }
}
