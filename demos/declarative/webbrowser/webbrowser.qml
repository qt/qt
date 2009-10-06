import Qt 4.6

import "content"
import "fieldtext"

Item {
    id: webBrowser

    property string urlString : "http://qt.nokia.com/"

    state: "Normal"

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
            x: -flickable.viewportX
            y: -flickable.viewportY
            width: webView.width*webView.scale
            height: flickable.y+webView.height*webView.scale
        }
        Item {
            id: headerSpace
            width: parent.width
            height: 60
            z: 1

            Rectangle {
                id: headerSpaceTint
                color: "black"
                opacity: 0
                anchors.fill: parent
             }

            Image {
                id: header
                source: "content/pics/header.png"
                width: parent.width
                height: 60
                state: "Normal"
                x: flickable.viewportX < 0 ? -flickable.viewportX : flickable.viewportX > flickable.viewportWidth-flickable.width
                                         ? -flickable.viewportX+flickable.viewportWidth-flickable.width : 0
                y: flickable.viewportY < 0 ? -flickable.viewportY : progressOff*
                                        (flickable.viewportY>height?-height:-flickable.viewportY)
                Text {
                    id: headerText

                    text: webView.title!='' || webView.progress == 1.0 ? webView.title : 'Loading...'
                    elide: "ElideRight"

                    color: "white"
                    styleColor: "black"
                    style: "Raised"

                    font.family: "Helvetica"
                    font.pointSize: 10
                    font.bold: true

                    anchors.left: header.left
                    anchors.right: header.right
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    anchors.top: header.top
                    anchors.topMargin: 4
                    horizontalAlignment: "AlignHCenter"
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
                            source: "content/pics/addressbar.sci"
                            anchors.fill: urlBox
                        }

                        BorderImage {
                            id: urlBoxhl
                            source: "content/pics/addressbar-filled.sci"
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
                        name: "Normal"
                        when: webView.progress == 1.0
                        PropertyChanges { target: header; progressOff: 1 }
                    },
                    State {
                        name: "ProgressShown"
                        when: webView.progress < 1.0
                        PropertyChanges { target: header; progressOff: 0; }
                    }
                ]
                transitions: [
                    Transition {
                        NumberAnimation {
                            target: header
                            properties: "progressOff"
                            easing: "easeInOutQuad"
                            duration: 300
                        }
                    }
                ]
            }
        }
        Flickable {
            id: flickable
            width: parent.width
            viewportWidth: Math.max(parent.width,webView.width*webView.scale)
            viewportHeight: Math.max(parent.height,webView.height*webView.scale)
            anchors.top: headerSpace.bottom
            anchors.bottom: footer.top
            anchors.left: parent.left
            anchors.right: parent.right
            pressDelay: 200

            WebView {
                id: webView
                pixelCacheSize: 4000000

                Script {
                    function fixUrl(url)
                    {
                        if (url == "") return url
                        if (url[0] == "/") return "file://"+url
                        if (url.indexOf(":")<0) {
                            if (url.indexOf(".")<0 || url.indexOf(" ")>=0) {
                                // Fall back to a search engine; hard-code Wikipedia
                                return "http://en.wikipedia.org/w/index.php?search="+url
                            } else {
                                return "http://"+url
                            }
                        }
                        return url
                    }
                }

                url: fixUrl(webBrowser.urlString)
                smooth: true
                fillColor: "white"
                focus: true

                preferredWidth: flickable.width
                webPageWidth: 980

                onUrlChanged: { if (url != null) { webBrowser.urlString = url.toString(); } }
                onDoubleClick: { heuristicZoom(clickX,clickY) }

                SequentialAnimation {
                    id: quickZoom

                    PropertyAction {
                        target: webView
                        property: "renderingEnabled"
                        value: false
                    }
                    ParallelAnimation {
                        NumberAnimation {
                            id: scaleAnim
                            target: webView
                            property: "scale"
                            from: 1
                            to: 0 // set before calling
                            easing: "easeLinear"
                            duration: 200
                        }
                        NumberAnimation {
                            id: flickVX
                            target: flickable
                            property: "viewportX"
                            easing: "easeLinear"
                            duration: 200
                            from: 0 // set before calling
                            to: 0 // set before calling
                        }
                        NumberAnimation {
                            id: flickVY
                            target: flickable
                            property: "viewportY"
                            easing: "easeLinear"
                            duration: 200
                            from: 0 // set before calling
                            to: 0 // set before calling
                        }
                    }
                    PropertyAction {
                        id: finalZoom
                        target: webView
                        property: "zoomFactor"
                    }
                    PropertyAction {
                        target: webView
                        property: "scale"
                        value: 1.0
                    }
                    // Have to set the viewportXY, since the above 2
                    // size changes may have started a correction if
                    // zoomFactor < 1.0.
                    PropertyAction {
                        id: finalX
                        target: flickable
                        property: "viewportX"
                        value: 0 // set before calling
                    }
                    PropertyAction {
                        id: finalY
                        target: flickable
                        property: "viewportY"
                        value: 0 // set before calling
                    }
                    PropertyAction {
                        target: webView
                        property: "renderingEnabled"
                        value: true
                    }
                }
                onZooming: {
                    if (centerX) {
                        sc = zoom/zoomFactor;
                        scaleAnim.to = sc;
                        flickVX.from = flickable.viewportX
                        flickVX.to = Math.min(Math.max(0,centerX-flickable.width/2),webView.width*sc-flickable.width)
                        finalX.value = Math.min(Math.max(0,centerX-flickable.width/2),webView.width*sc-flickable.width)
                        flickVY.from = flickable.viewportY
                        flickVY.to = Math.min(Math.max(0,centerY-flickable.height/2),webView.height*sc-flickable.height)
                        finalY.value = Math.min(Math.max(0,centerY-flickable.height/2),webView.height*sc-flickable.height)
                        finalZoom.value = zoom
                        quickZoom.start()
                    }
                }
            }
            Rectangle {
                id: webViewTint
                color: "black"
                opacity: 0
                anchors.fill: webView
                /*MouseRegion {
                    anchors.fill: WebViewTint
                    onClicked: { proxy.focus=false }
                }*/
            }
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
                            properties: "opacity"
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
                            properties: "opacity"
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
