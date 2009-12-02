import Qt 4.6

import "content"

Item {
    id: webBrowser

    property string urlString : "http://qt.nokia.com/"

    width: 640
    height: 480

    Item {
        id: webPanel
        anchors.fill: parent
        clip: true
        Rectangle {
            color: "#555555"
            anchors.fill: parent
        }
        Image {
            source: "content/pics/softshadow-bottom.png"
            width: webPanel.width
            height: 16
        }
        Image {
            source: "content/pics/softshadow-top.png"
            width: webPanel.width
            height: 16
            anchors.bottom: footer.top
        }
        RectSoftShadow {
            x: -webView.viewportX
            y: -webView.viewportY
            width: webView.viewportWidth
            height: webView.viewportHeight+headerSpace.height
        }
        Item {
            id: headerSpace
            width: parent.width
            height: 60
            z: 1

            RetractingWebBrowserHeader { id: header }
        }
        FlickableWebView {
            id: webView
            width: parent.width
            anchors.top: headerSpace.bottom
            anchors.bottom: footer.top
            anchors.left: parent.left
            anchors.right: parent.right
        }
        BorderImage {
            id: footer
            source: "content/pics/footer.sci"
            width: parent.width
            height: 43
            anchors.bottom: parent.bottom
            Rectangle {
                y: -1
                width: parent.width
                height: 1
                color: "#555555"
            }
            Item {
                id: backbutton
                width: back_e.width
                height: back_e.height
                anchors.right: reload.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    id: back_e
                    source: "content/pics/back.png"
                    anchors.fill: parent
                }
                Image {
                    id: back_d
                    source: "content/pics/back-disabled.png"
                    anchors.fill: parent
                }
                states: [
                    State {
                        name: "Enabled"
                        when: webView.back.enabled==true
                        PropertyChanges { target: back_e; opacity: 1 }
                        PropertyChanges { target: back_d; opacity: 0 }
                    },
                    State {
                        name: "Disabled"
                        when: webView.back.enabled==false
                        PropertyChanges { target: back_e; opacity: 0 }
                        PropertyChanges { target: back_d; opacity: 1 }
                    }
                ]
                transitions: [
                    Transition {
                        NumberAnimation {
                            matchProperties: "opacity"
                            easing: "easeInOutQuad"
                            duration: 300
                        }
                    }
                ]
                MouseRegion {
                    anchors.fill: back_e
                    onClicked: { if (webView.back.enabled) webView.back.trigger() }
                }
            }
            Image {
                id: reload
                source: "content/pics/reload.png"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
            MouseRegion {
                anchors.fill: reload
                onClicked: { webView.reload.trigger() }
            }
            Item {
                id: forwardbutton
                width: forward_e.width
                height: forward_e.height
                anchors.left: reload.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    id: forward_e
                    source: "content/pics/forward.png"
                    anchors.fill: parent
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: forward_d
                    source: "content/pics/forward-disabled.png"
                    anchors.fill: parent
                }
                states: [
                    State {
                        name: "Enabled"
                        when: webView.forward.enabled==true
                        PropertyChanges { target: forward_e; opacity: 1 }
                        PropertyChanges { target: forward_d; opacity: 0 }
                    },
                    State {
                        name: "Disabled"
                        when: webView.forward.enabled==false
                        PropertyChanges { target: forward_e; opacity: 0 }
                        PropertyChanges { target: forward_d; opacity: 1 }
                    }
                ]
                transitions: [
                    Transition {
                        NumberAnimation {
                            matchProperties: "opacity"
                            easing: "easeInOutQuad"
                            duration: 320
                        }
                    }
                ]
                MouseRegion {
                    anchors.fill: parent
                    onClicked: { if (webView.forward.enabled) webView.forward.trigger() }
                }
            }
        }
    }
}
