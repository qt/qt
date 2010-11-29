import QtQuick 1.1

Rectangle {
    width: 640
    height: 360
    color: "gray"

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: 500
        contentHeight: 500

        PinchArea {
            width: Math.max(flick.contentWidth, flick.width)
            height: Math.max(flick.contentHeight, flick.height)
            onPinchChanged: {
                // adjust content pos due to drag
                flick.contentX += pinch.lastCenter.x - pinch.center.x
                flick.contentY += pinch.lastCenter.y - pinch.center.y

                // resize content
                var scale = 1.0 + pinch.scale - pinch.lastScale
                flick.resizeContent(flick.contentWidth * scale, flick.contentHeight * scale, pinch.center)
            }

            onPinchFinished: {
                // Move its content within bounds.
                flick.returnToBounds()
            }

            Rectangle {
                width: flick.contentWidth
                height: flick.contentHeight
                color: "white"
                Image {
                    anchors.fill: parent
                    source: "qt-logo.jpg"
                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            flick.contentWidth = 500
                            flick.contentHeight = 500
                        }
                    }
                }
            }
        }
    }
}
