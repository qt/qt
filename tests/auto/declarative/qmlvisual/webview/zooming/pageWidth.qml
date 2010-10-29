import QtQuick 1.0
import org.webkit 1.0

WebView {
    property string skip: "WebView tests not counting until resources allocated to WebView maintenance"
    width: 200
    height: 250
    url: "resolution.html"
    webPageWidth: 400
    preferredWidth: 200
}
