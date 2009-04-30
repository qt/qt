import "content"
Item {
    width: 640
    height: 480
    id: WebBrowser
    state: "Normal"
    properties: Property {
        name: "url"
        value: "http://www.qtsoftware.com"
    }
    Script {

        function zoomOut() {
            WebBrowser.state = "ZoomedOut";
        }
        function toggleZoom() {
            if(WebBrowser.state == "ZoomedOut") {
                Flick.centerX = WebView.mouseX;
                Flick.centerY = WebView.mouseY;
                WebBrowser.state = "Normal";
            } else {
                zoomOut();
            }
        }
    
    }
    Item {
        id: WebPanel
        anchors.fill: parent
        clip: true
        Rect {
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
            anchors.bottom: Footer.top
            height: 16
        }
        RectSoftShadow {
            x: -Flick.xPosition
            y: -Flick.yPosition
            width: WebView.width*WebView.scale
            height: Flick.y+WebView.height*WebView.scale
        }
        Item {
            id: HeaderSpace
            width: parent.width
            height: 60
            z: 1
            Image {
                id: Header
                width: parent.width
                state: "Normal"
                x: Flick.xPosition < 0                                 ? -Flick.xPosition                                 : Flick.xPosition > Flick.viewportWidth-Flick.width                                     ? -Flick.xPosition+Flick.viewportWidth-Flick.width                                     : 0
                y: Flick.yPosition < 0 ? -Flick.yPosition : progressOff*(Flick.yPosition>height?-height:-Flick.yPosition)
                height: 64
                source: "content/pics/header.png"
                Text {
                    id: HeaderText
                    text: WebView.title!='' || WebView.progress == 1.0 ? WebView.title : 'Loading...'
                    color: "white"
                    styleColor: "black"
                    style: Raised
                    font.family: "Helvetica"
                    font.size: 10
                    font.bold: true
                    elide: "ElideRight"
                    anchors.left: Header.left
                    anchors.right: Header.right
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    anchors.top: Header.top
                    anchors.topMargin: 4
                    hAlign: AlignHCenter
                }
                Item {
                    anchors.top: HeaderText.bottom
                    anchors.topMargin: 2
                    anchors.bottom: parent.bottom
                    width: parent.width
                    Item {
                        id: UrlBox
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        height: 31
                        anchors.top: parent.top
                        clip: true
                        Image {
                            source: "content/pics/addressbar.sci"
                            anchors.fill: UrlBox
                        }
                        Image {
                            id: UrlBoxhl
                            source: "content/pics/addressbar-filled.sci"
                            opacity: 1-Header.progressOff
                            clip: true
                            width: parent.width*WebView.progress
                            height: parent.height
                        }
                        KeyProxy {
                            id: proxy
                            anchors.left: UrlBox.left
                            anchors.fill: UrlBox
                            focusable: true
                            targets: {[keyActions,EditUrl]}
                        }
                        KeyActions {
                            id: keyActions
                            keyReturn: "WebBrowser.url = EditUrl.text; proxy.focus=false;"
                        }
                        TextEdit {
                            id: EditUrl
                            color: "#555555"
                            text: WebView.url == '' ? ' ' : WebView.url
                            anchors.left: UrlBox.left
                            anchors.right: UrlBox.right
                            anchors.leftMargin: 6
                            anchors.verticalCenter: UrlBox.verticalCenter
                            anchors.verticalCenterOffset: 1
                            font.size: 11
                            wrap: false
                            opacity: 0
                        }
                        Text {
                            id: ShowUrl
                            color: "#555555"
                            text: WebView.url == '' ? ' ' : WebView.url
                            anchors.left: UrlBox.left
                            anchors.right: UrlBox.right
                            anchors.leftMargin: 6
                            anchors.verticalCenter: UrlBox.verticalCenter
                            anchors.verticalCenterOffset: 1
                            font.size: 11
                        }
                    }
                    MouseRegion {
                        anchors.fill: UrlBox
                        onClicked: { proxy.focus=true }
                    }
                }
                properties: Property {
                    name: "progressOff"
                    value: 1
                    type: "Real"
                }
                states: [
                    State {
                        name: "Normal"
                        when: WebView.progress == 1.0
                        SetProperty {
                            target: Header
                            property: "progressOff"
                            value: 1
                        }
                    },
                    State {
                        name: "ProgressShown"
                        when: WebView.progress < 1.0
                        SetProperty {
                            target: Header
                            property: "progressOff"
                            value: 0
                        }
                    }
                ]
                transitions: [
                    Transition {
                        NumericAnimation {
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
            anchors.top: HeaderSpace.bottom
            anchors.bottom: Footer.top
            anchors.left: parent.left
            anchors.right: parent.right
            width: parent.width
            viewportWidth: Math.max(parent.width,WebView.width*WebView.scale)
            viewportHeight: Math.max(parent.height,WebView.height*WebView.scale)
            properties: Property {
                name: "centerX"
                value: 0
                type: "Real"
            }
            properties: Property {
                name: "centerY"
                value: 0
                type: "Real"
            }
            WebView {
                id: WebView
                cacheSize: 4000000
                smooth: true
                url: WebBrowser.url
                onDoubleClick: { toggleZoom() }
                focusable: true
                focus: true
                idealWidth: Flick.width
                idealHeight: Flick.height/scale
                onUrlChanged: { Flick.xPosition=0; Flick.yPosition=0; zoomOut() }
                scale: (width > 0) ? Flick.width/width*zoomedOut+(1-zoomedOut) : 1
                properties: Property {
                    name: "zoomedOut"
                    type: "real"
                    value: 1
                }
            }
            Rect {
                id: WebViewTint
                anchors.fill: WebView
                color: "black"
                opacity: 0
                MouseRegion {
                    anchors.fill: WebViewTint
                    onClicked: { proxy.focus=false }
                }
            }
        }
        Image {
            id: Footer
            width: parent.width
            anchors.bottom: parent.bottom
            height: 43
            source: "content/pics/footer.sci"
            Rect {
                y: -1
                width: parent.width
                height: 1
                color: "#555555"
            }
            Item {
                id: backbutton
                anchors.right: reload.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width: back_e.width
                height: back_e.height
                Image {
                    anchors.fill: parent
                    id: back_e
                    source: "content/pics/back.png"
                }
                Image {
                    anchors.fill: parent
                    id: back_d
                    source: "content/pics/back-disabled.png"
                }
                states: [
                    State {
                        name: "Enabled"
                        when: WebView.back.enabled==true
                        SetProperty {
                            target: back_e
                            property: "opacity"
                            value: 1
                        }
                        SetProperty {
                            target: back_d
                            property: "opacity"
                            value: 0
                        }
                    },
                    State {
                        name: "Disabled"
                        when: WebView.back.enabled==false
                        SetProperty {
                            target: back_e
                            property: "opacity"
                            value: 0
                        }
                        SetProperty {
                            target: back_d
                            property: "opacity"
                            value: 1
                        }
                    }
                ]
                transitions: [
                    Transition {
                        NumericAnimation {
                            properties: "opacity"
                            easing: "easeInOutQuad"
                            duration: 300
                        }
                    }
                ]
                MouseRegion {
                    anchors.fill: back_e
                    onClicked: { if (WebView.back.enabled) WebView.back.trigger() }
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
                onClicked: { WebView.reload.trigger() }
            }
            Item {
                id: forwardbutton
                anchors.left: reload.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width: forward_e.width
                height: forward_e.height
                Image {
                    anchors.fill: parent
                    anchors.verticalCenter: parent.verticalCenter
                    id: forward_e
                    source: "content/pics/forward.png"
                }
                Image {
                    anchors.fill: parent
                    id: forward_d
                    source: "content/pics/forward-disabled.png"
                }
                states: [
                    State {
                        name: "Enabled"
                        when: WebView.forward.enabled==true
                        SetProperty {
                            target: forward_e
                            property: "opacity"
                            value: 1
                        }
                        SetProperty {
                            target: forward_d
                            property: "opacity"
                            value: 0
                        }
                    },
                    State {
                        name: "Disabled"
                        when: WebView.forward.enabled==false
                        SetProperty {
                            target: forward_e
                            property: "opacity"
                            value: 0
                        }
                        SetProperty {
                            target: forward_d
                            property: "opacity"
                            value: 1
                        }
                    }
                ]
                transitions: [
                    Transition {
                        NumericAnimation {
                            properties: "opacity"
                            easing: "easeInOutQuad"
                            duration: 320
                        }
                    }
                ]
                MouseRegion {
                    anchors.fill: parent
                    onClicked: { if (WebView.forward.enabled) WebView.forward.trigger() }
                }
            }
        }
    }
    states: [
        State {
            name: "Normal"
            SetProperty {
                target: WebView
                property: "zoomedOut"
                value: 0
            }
            SetProperty {
                target: Flick
                property: "xPosition"
                value: Math.min(WebView.width-Flick.width,Math.max(0,Flick.centerX-Flick.width/2))
            }
            SetProperty {
                target: Flick
                property: "yPosition"
                value: Math.min(WebView.height-Flick.height,Math.max(0,Flick.centerY-Flick.height/2))
            }
        },
        State {
            name: "ZoomedOut"
            SetProperty {
                target: WebView
                property: "zoomedOut"
                value: 1
            }
        }
    ]
    transitions: [
        Transition {
            SequentialAnimation {
                SetPropertyAction {
                    target: WebView
                    property: "smooth"
                    value: false
                }
                ParallelAnimation {
                    NumericAnimation {
                        target: WebView
                        properties: "zoomedOut"
                        easing: "easeInOutQuad"
                        duration: 200
                    }
                    NumericAnimation {
                        target: Flick
                        properties: "xPosition,yPosition"
                        easing: "easeInOutQuad"
                        duration: 200
                    }
                }
                SetPropertyAction {
                    target: WebView
                    property: "smooth"
                    value: true
                }
            }
        }
    ]
}
