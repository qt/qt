import Qt 4.7
import org.webkit 1.0

Flickable {
    property alias title: webView.title
    property alias icon: webView.icon
    property alias progress: webView.progress
    property alias url: webView.url
    property alias back: webView.back
    property alias reload: webView.reload
    property alias forward: webView.forward

    id: flickable
    width: parent.width
    contentWidth: Math.max(parent.width,webView.width*webView.scale)
    contentHeight: Math.max(parent.height,webView.height*webView.scale)
    anchors.top: headerSpace.bottom
    anchors.bottom: footer.top
    anchors.left: parent.left
    anchors.right: parent.right
    pressDelay: 200

    WebView {
        id: webView
        pixelCacheSize: 4000000
        transformOrigin: Item.TopLeft

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

        url: fixUrl(webBrowser.urlString)
        smooth: false // We don't want smooth scaling, since we only scale during (fast) transitions
        smoothCache: true // We do want smooth rendering
        fillColor: "white"
        focus: true
        zoomFactor: 1

        onAlert: console.log(message)

        function doZoom(zoom,centerX,centerY)
        {
            if (centerX) {
                var sc = zoom/contentsScale;
                scaleAnim.to = sc;
                flickVX.from = flickable.contentX
                flickVX.to = Math.max(0,Math.min(centerX-flickable.width/2,webView.width*sc-flickable.width))
                finalX.value = flickVX.to
                flickVY.from = flickable.contentY
                flickVY.to = Math.max(0,Math.min(centerY-flickable.height/2,webView.height*sc-flickable.height))
                finalY.value = flickVY.to
                finalZoom.value = zoom
                quickZoom.start()
            }
        }

        Keys.onLeftPressed: webView.contentsScale -= 0.1
        Keys.onRightPressed: webView.contentsScale += 0.1

        preferredWidth: flickable.width*zoomFactor
        preferredHeight: flickable.height*zoomFactor
        contentsScale: 1/zoomFactor
        onContentsSizeChanged: {
            // zoom out
            contentsScale = Math.min(1,flickable.width / contentsSize.width)
        }
        onUrlChanged: {
            // got to topleft
            flickable.contentX = 0
            flickable.contentY = 0
            if (url != null) { header.editUrl = url.toString(); }
        }
        onDoubleClick: {
                        if (!heuristicZoom(clickX,clickY,2.5)) {
                            var zf = flickable.width / contentsSize.width
                            if (zf >= contentsScale)
                                zf = 2.0/zoomFactor // zoom in (else zooming out)
                            doZoom(zf,clickX*zf,clickY*zf)
                         }
                       }

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
                    easing.type: "Linear"
                    duration: 200
                }
                NumberAnimation {
                    id: flickVX
                    target: flickable
                    property: "contentX"
                    easing.type: "Linear"
                    duration: 200
                    from: 0 // set before calling
                    to: 0 // set before calling
                }
                NumberAnimation {
                    id: flickVY
                    target: flickable
                    property: "contentY"
                    easing.type: "Linear"
                    duration: 200
                    from: 0 // set before calling
                    to: 0 // set before calling
                }
            }
            PropertyAction {
                id: finalZoom
                target: webView
                property: "contentsScale"
            }
            PropertyAction {
                target: webView
                property: "scale"
                value: 1.0
            }
            // Have to set the contentXY, since the above 2
            // size changes may have started a correction if
            // contentsScale < 1.0.
            PropertyAction {
                id: finalX
                target: flickable
                property: "contentX"
                value: 0 // set before calling
            }
            PropertyAction {
                id: finalY
                target: flickable
                property: "contentY"
                value: 0 // set before calling
            }
            PropertyAction {
                target: webView
                property: "renderingEnabled"
                value: true
            }
        }
        onZoomTo: doZoom(zoom,centerX,centerY)
    }
}
