import QtQuick 1.0
import QtWebKit 1.0

Grid {
    property string skip: "WebView tests not counting until resources allocated to WebView maintenance"
    columns: 2
    Rectangle {
        Text { id: label; x:10; y:170; color: "green"; text: "No image" }
        border.color: "black"
        width: 200
        height: 200
        WebView {
            anchors.fill: parent
            settings.autoLoadImages: false
            url: "test-img.html"
            MouseArea {
                anchors.fill: parent
                onClicked: { parent.settings.autoLoadImages=true; label.text=""; parent.reload.trigger() }
            }
        }
    }
}
