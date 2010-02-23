import Qt 4.6

import "fieldtext"

Image {
    property alias editUrl: editUrl.text

    id: header
    source: "pics/header.png"
    width: parent.width
    height: 60
    x: webView.viewportX < 0 ? -webView.viewportX : webView.viewportX > webView.viewportWidth-webView.width
                             ? -webView.viewportX+webView.viewportWidth-webView.width : 0
    y: webView.viewportY < 0 ? -webView.viewportY : progressOff*
                            (webView.viewportY>height?-height:-webView.viewportY)
    Text {
        id: headerText

        text: webView.title!='' || webView.progress == 1.0 ? webView.title : 'Loading...'
        elide: Text.ElideRight

        color: "white"
        styleColor: "black"
        style: Text.Raised

        font.family: "Helvetica"
        font.pointSize: 10
        font.bold: true

        anchors.left: header.left
        anchors.right: header.right
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        anchors.top: header.top
        anchors.topMargin: 4
        horizontalAlignment: Text.AlignHCenter
    }
    Item {
        width: parent.width
        anchors.top: headerText.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom

        Item {
            id: urlBox
            height: 31
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.right: parent.right
            anchors.rightMargin: 12
            anchors.top: parent.top
            clip: true
            property bool mouseGrabbed: false

            BorderImage {
                source: "pics/addressbar.sci"
                anchors.fill: urlBox
            }

            BorderImage {
                id: urlBoxhl
                source: "pics/addressbar-filled.sci"
                width: parent.width*webView.progress
                height: parent.height
                opacity: 1-header.progressOff
                clip: true
            }

            FieldText {
                id: editUrl
                mouseGrabbed: parent.mouseGrabbed

                text: webBrowser.urlString
                label: "url:"
                onConfirmed: { webBrowser.urlString = editUrl.text; webView.focus=true }
                onCancelled: { webView.focus=true }
                onStartEdit: { webView.focus=false }

                anchors.left: urlBox.left
                anchors.right: urlBox.right
                anchors.leftMargin: 6
                anchors.verticalCenter: urlBox.verticalCenter
                anchors.verticalCenterOffset: 1
            }
        }
    }

    property real progressOff : 1
    states: [
        State {
            name: "ProgressShown"
            when: webView.progress < 1.0
            PropertyChanges { target: header; progressOff: 0; }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation {
                targets: header
                properties: "progressOff"
                easing.type: "InOutQuad"
                duration: 300
            }
        }
    ]
}
