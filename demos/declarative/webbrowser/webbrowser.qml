import Qt 4.6

import "content"
import "fieldtext"

Item {
    id: WebBrowser

    property string urlString : "http://qt.nokia.com/"

    state: "Normal"

    width: 640
    height: 480

    Script {
        function zoomOut() {
            WebBrowser.state = "ZoomedOut";
        }
        function toggleZoom() {
            if(WebBrowser.state == "ZoomedOut") {
                Flick.centerX = MyWebView.mouseX;
                Flick.centerY = MyWebView.mouseY;
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
            height: 16
            anchors.bottom: Footer.top
        }
        RectSoftShadow {
            x: -Flick.xPosition
            y: -Flick.yPosition
            width: MyWebView.width*MyWebView.scale
            height: Flick.y+MyWebView.height*MyWebView.scale
        }
        Item {
            id: HeaderSpace
            width: parent.width
            height: 60
            z: 1

            Rect {
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
                x: Flick.xPosition < 0 ? -Flick.xPosition : Flick.xPosition > Flick.viewportWidth-Flick.width
                                         ? -Flick.xPosition+Flick.viewportWidth-Flick.width : 0
                y: Flick.yPosition < 0 ? -Flick.yPosition : progressOff*
                                        (Flick.yPosition>height?-height:-Flick.yPosition)
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
                    hAlign: "AlignHCenter"
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

                            /*<<<<<<< HEAD:demos/declarative/webbrowser/webbrowser.qml
                            text: MyWebView.url == '' ? ' ' : MyWebView.url
                            wrap: false
                            font.pointSize: 11
                            color: "#555555"
                            focusOnPress: true
                            =======*/
                            text: WebBrowser.urlString
                            label: "url:"
                            onConfirmed: { print ('OnConfirmed: '+EditUrl.text); WebBrowser.urlString = EditUrl.text; print (EditUrl.text); MyWebView.focus=true }
                            onCancelled: { MyWebView.focus=true }
                            onStartEdit: { print (EditUrl.text); MyWebView.focus=false }

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
                        SetProperties { target: Header; progressOff: 1 }
                    },
                    State {
                        name: "ProgressShown"
                        when: MyWebView.progress < 1.0
                        SetProperties { target: Header; progressOff: 0; }
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

            property real centerX : 0
            property real centerY : 0

            WebView {
                id: MyWebView
                cacheSize: 4000000

                url: WebBrowser.urlString
                smooth: !Flick.moving
                fillColor: "white"
                focus: true
                interactive: true

                idealWidth: Flick.width
                idealHeight: Flick.height/scale
                scale: (width > 0) ? Flick.width/width*zoomedOut+(1-zoomedOut) : 1

                onUrlChanged: { print ('OnUrlChanged: '+url.toString()); WebBrowser.urlString = url.toString(); print ('Moved to url: ' + WebBrowser.urlString) }
                onDoubleClick: { toggleZoom() }

                property real zoomedOut : 1
            }
            Rect {
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
            Rect {
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
                        SetProperties { target: back_e; opacity: 1 }
                        SetProperties { target: back_d; opacity: 0 }
                    },
                    State {
                        name: "Disabled"
                        when: MyWebView.back.enabled==false
                        SetProperties { target: back_e; opacity: 0 }
                        SetProperties { target: back_d; opacity: 1 }
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
                        SetProperties { target: forward_e; opacity: 1 }
                        SetProperties { target: forward_d; opacity: 0 }
                    },
                    State {
                        name: "Disabled"
                        when: MyWebView.forward.enabled==false
                        SetProperties { target: forward_e; opacity: 0 }
                        SetProperties { target: forward_d; opacity: 1 }
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
    states: [
        State {
            name: "Normal"
            SetProperties { target: MyWebView; zoomedOut: 0 }
            SetProperties { target: Flick; explicit: true; xPosition: Math.min(MyWebView.width-Flick.width,Math.max(0,Flick.centerX-Flick.width/2)) }
            SetProperties { target: Flick; explicit: true; yPosition: Math.min(MyWebView.height-Flick.height,Math.max(0,Flick.centerY-Flick.height/2)) }
        },
        State {
            name: "ZoomedOut"
            SetProperties { target: MyWebView; zoomedOut: 1 }
        }
    ]
    transitions: [
        Transition {
            SequentialAnimation {
                SetPropertyAction {
                    target: MyWebView
                    property: "smooth"
                    value: false
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: MyWebView
                        properties: "zoomedOut"
                        easing: "easeInOutQuad"
                        duration: 200
                    }
                    NumberAnimation {
                        target: Flick
                        properties: "xPosition,yPosition"
                        easing: "easeInOutQuad"
                        duration: 200
                    }
                }
                SetPropertyAction {
                    target: MyWebView
                    property: "smooth"
                    value: !Flick.moving
                }
            }
        }
    ]
}
