import Qt 4.7
import org.webkit 1.0

Column {
    WebView {
        id: webview
        width: 200
        height: 200
        url: "test-objects.html"
        javaScriptWindowObjects:
            QtObject {
                property string text: btntext.text
                WebView.windowObjectName: "qmltext"
                onTextChanged: {
                    webview.evaluateJavaScript("{document.getElementById('button').value=window.qmltext.text}")
                }
            }
    }
    Row {
        Text { text: "Input:" }
        Rectangle {
            width: btntext.width+10
            height: btntext.height+10
            border.color: "black"
            TextInput {
                id: btntext
                text: "Blah"
                cursorDelegate: Rectangle { width: 1; color: "red" }
            }
        }
    }
}
