import Qt 4.6

import "content"
import "fieldtext"

Item {
    id: WebBrowser

    property string urlString : "http://qt.nokia.com/"

    state: "Normal"

    width: 640
    height: 480

    Item {
        id: WebPanel
        anchors.fill: parent
        clip: true
        Rectangle {
            color: "#555555"
            anchors.fill: parent
        }
        Image {
            source: "content/pics/softshadow-bottom.png"
            width: WebPanel.width
            height: 16
        }
        Image {
            source: "content/pics/softshadow-top.png"
            width: WebPanel.width
            height: 16
            anchors.bottom: Footer.top
        }
        RectSoftShadow {
            x: -Flick.viewportX
            y: -Flick.viewportY
            width: MyWebView.width*MyWebView.scale
            height: Flick.y+MyWebView.height*MyWebView.scale
        }
        Item {
            id: HeaderSpace
            width: parent.width
            height: 60
            z: 1

            Rectangle {
                id: HeaderSpaceTint
                color: "black"
                opacity: 0
                anchors.fill: parent
             }

            Image {
                id: Header
                source: "content/pics/header.png"
                width: parent.width
                height: 64
                state: "Normal"
                x: Flick.viewportX < 0 ? -Flick.viewportX : Flick.viewportX > Flick.viewportWidth-Flick.width
                                         ? -Flick.viewportX+Flick.viewportWidth-Flick.width : 0
                y: Flick.viewportY < 0 ? -Flick.viewportY : progressOff*
                                        (Flick.viewportY>height?-height:-Flick.viewportY)
                Text {
                    id: HeaderText

                    text: MyWebView.title!='' || MyWebView.progress == 1.0 ? MyWebView.title : 'Loading...'
                    elide: "ElideRight"

                    color: "white"
                    styleColor: "black"
                    style: "Raised"

                    font.family: "Helvetica"
                    font.pointSize: 10
                    font.bold: true

                    anchors.left: Header.left
                    anchors.right: Header.right
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    anchors.top: Header.top
                    anchors.topMargin: 4
                    horizontalAlignment: "AlignHCenter"
                }
                Item {
                    width: parent.width
                    anchors.top: HeaderText.bottom
                    anchors.topMargin: 2
                    anchors.bottom: parent.bottom

                    Item {
                        id: UrlBox
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
                            anchors.fill: UrlBox
                        }
                                                
                        BorderImage {
                            id: UrlBoxhl
                            source: "content/pics/addressbar-filled.sci"
                            width: parent.width*MyWebView.progress
                            height: parent.height
                            opacity: 1-Header.progressOff
                            clip: true
                        }

                        FieldText {
                            id: EditUrl
                            mouseGrabbed: parent.mouseGrabbed

                            text: WebBrowser.urlString
                            label: "url:"
                            onConfirmed: { WebBrowser.urlString = EditUrl.text; MyWebView.focus=true }
                            onCancelled: { MyWebView.focus=true }
                            onStartEdit: { MyWebView.focus=false }

                            anchors.left: UrlBox.left
                            anchors.right: UrlBox.right
                            anchors.leftMargin: 6
                            anchors.verticalCenter: UrlBox.verticalCenter
                            anchors.verticalCenterOffset: 1
                        }
                    }
                }

                property real progressOff : 1
                states: [
                    State {
                        name: "Normal"
                        when: MyWebView.progress == 1.0
                        PropertyChanges { target: Header; progressOff: 1 }
                    },
                    State {
                        name: "ProgressShown"
                        when: MyWebView.progress < 1.0
                        PropertyChanges { target: Header; progressOff: 0; }
                    }
                ]
                transitions: [
                    Transition {
                        NumberAnimation {
                            target: Header
                            properties: "progressOff"
                            easing: "easeInOutQuad"
                            duration: 300
                        }
                    }
                ]
            }
        }
        Flickable {
            id: Flick
            width: parent.width
            viewportWidth: Math.max(parent.width,MyWebView.width*MyWebView.scale)
            viewportHeight: Math.max(parent.height,MyWebView.height*MyWebView.scale)
            anchors.top: HeaderSpace.bottom
            anchors.bottom: Footer.top
            anchors.left: parent.left
            anchors.right: parent.right
            pressDelay: 200

            WebView {
                id: MyWebView
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

                url: fixUrl(WebBrowser.urlString)
                smooth: true
                fillColor: "white"
                focus: true

                preferredWidth: Flick.width
                webPageWidth: 980

                onUrlChanged: { if (url != null) { WebBrowser.urlString = url.toString(); } }
                onDoubleClick: { heuristicZoom(clickX,clickY) }

                SequentialAnimation {
                    id: QuickZoom

                    PropertyAction {
                        target: MyWebView
                        property: "renderingEnabled"
                        value: false
                    }
                    ParallelAnimation {
                        NumberAnimation {
                            id: ScaleAnim
                            target: MyWebView
                            property: "scale"
                            from: 1
                            to: 0 // set before calling
                            easing: "easeLinear"
                            duration: 200
                        }
                        NumberAnimation {
                            id: FlickVX
                            target: Flick
                            property: "viewportX"
                            easing: "easeLinear"
                            duration: 200
                            from: 0 // set before calling
                            to: 0 // set before calling
                        }
                        NumberAnimation {
                            id: FlickVY
                            target: Flick
                            property: "viewportY"
                            easing: "easeLinear"
                            duration: 200
                            from: 0 // set before calling
                            to: 0 // set before calling
                        }
                    }
                    PropertyAction {
                        id: FinalZoom
                        target: MyWebView
                        property: "zoomFactor"
                    }
                    PropertyAction {
                        target: MyWebView
                        property: "scale"
                        value: 1.0
                    }
                    // Have to set the viewportXY, since the above 2
                    // size changes may have started a correction if
                    // zoomFactor < 1.0.
                    PropertyAction {
                        id: FinalX
                        target: Flick
                        property: "viewportX"
                        value: 0 // set before calling
                    }
                    PropertyAction {
                        id: FinalY
                        target: Flick
                        property: "viewportY"
                        value: 0 // set before calling
                    }
                    PropertyAction {
                        target: MyWebView
                        property: "renderingEnabled"
                        value: true
                    }
                }
                onZooming: {
                    if (centerX) {
                        sc = zoom/zoomFactor;
                        ScaleAnim.to = sc;
                        FlickVX.from = Flick.viewportX
                        FlickVX.to = Math.min(Math.max(0,centerX-Flick.width/2),MyWebView.width*sc-Flick.width)
                        FinalX.value = Math.min(Math.max(0,centerX-Flick.width/2),MyWebView.width*sc-Flick.width)
                        FlickVY.from = Flick.viewportY
                        FlickVY.to = Math.min(Math.max(0,centerY-Flick.height/2),MyWebView.height*sc-Flick.height)
                        FinalY.value = Math.min(Math.max(0,centerY-Flick.height/2),MyWebView.height*sc-Flick.height)
                        FinalZoom.value = zoom
                        QuickZoom.start()
                    }
                }
            }
            Rectangle {
                id: WebViewTint
                color: "black"
                opacity: 0
                anchors.fill: MyWebView
                /*MouseRegion {
                    anchors.fill: WebViewTint
                    onClicked: { proxy.focus=false }
                }*/
            }
        }
        BorderImage {
            id: Footer
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
                        when: MyWebView.back.enabled==true
                        PropertyChanges { target: back_e; opacity: 1 }
                        PropertyChanges { target: back_d; opacity: 0 }
                    },
                    State {
                        name: "Disabled"
                        when: MyWebView.back.enabled==false
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
                    onClicked: { if (MyWebView.back.enabled) MyWebView.back.trigger() }
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
                onClicked: { MyWebView.reload.trigger() }
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
                        when: MyWebView.forward.enabled==true
                        PropertyChanges { target: forward_e; opacity: 1 }
                        PropertyChanges { target: forward_d; opacity: 0 }
                    },
                    State {
                        name: "Disabled"
                        when: MyWebView.forward.enabled==false
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
                    onClicked: { if (MyWebView.forward.enabled) MyWebView.forward.trigger() }
                }
            }
        }
    }
}
