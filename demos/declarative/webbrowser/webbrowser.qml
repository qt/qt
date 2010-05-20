import Qt 4.7
import org.webkit 1.0

import "content"

Rectangle {
    id: webBrowser

    property string urlString : "http://qt.nokia.com/"

    width: 800; height: 600
    color: "#343434"

    FlickableWebView {
        id: webView
        url: webBrowser.urlString
        anchors { top: headerSpace.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
    }

    Item { id: headerSpace; width: parent.width; height: 62 }

    Header {
        id: header
        editUrl: webBrowser.urlString
        width: headerSpace.width; height: headerSpace.height
    }

    ScrollBar {
        scrollArea: webView; width: 8
        anchors { right: parent.right; top: header.bottom; bottom: parent.bottom }
    }

    ScrollBar {
        scrollArea: webView; height: 8; orientation: Qt.Horizontal
        anchors { right: parent.right; rightMargin: 8; left: parent.left; bottom: parent.bottom }
    }
}
