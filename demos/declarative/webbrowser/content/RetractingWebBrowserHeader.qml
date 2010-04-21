import Qt 4.7

import "fieldtext"

Image {
    property alias editUrl: editUrl.text

    id: header
    source: "pics/header.png"
    width: parent.width
    height: 60
    x: webView.contentX < 0 ? -webView.contentX : webView.contentX > webView.contentWidth-webView.width
                             ? -webView.contentX+webView.contentWidth-webView.width : 0
    y: webView.contentY < 0 ? -webView.contentY : progressOff*
                            (webView.contentY>height?-height:-webView.contentY)
    Row {
        id: headerTitle

        anchors.top: header.top
        anchors.topMargin: 4
        x: parent.width > headerIcon.width+headerText.width+6 ? (parent.width-headerIcon.width-headerText.width-6)/2 : 0
        spacing: 6

        Image {
            id: headerIcon
            pixmap: webView.icon
        }

        Text {
            id: headerText

            text: webView.title!='' || webView.progress == 1.0 ? webView.title : 'Loading...'

            color: "white"
            styleColor: "black"
            style: Text.Raised

            font.family: "Helvetica"
            font.pointSize: 10
            font.bold: true

            horizontalAlignment: Text.AlignHCenter
        }
    }
    Item {
        width: parent.width
        anchors.top: headerTitle.bottom
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
