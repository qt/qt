// Demonstrates opening new WebViews from HTML
//
// Note that to open windows from JavaScript, you will need to
// allow it on WebView with settings.javascriptCanOpenWindows: true

import Qt 4.6

Row {
    id: Pages
    height: 200
    resources: [
        Component {
            id: WebViewPage
                Rectangle {
                    width: WV.width
                    height: WV.height
                    WebView {
                        id: WV
                        newWindowComponent: WebViewPage
                        newWindowParent: Pages
                        url: "newwindows.html"
                    }
                }
        }
    ]
    width: 500
    Loader { sourceComponent: WebViewPage }
}
