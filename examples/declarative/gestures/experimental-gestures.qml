import Qt 4.7
import Qt.labs.gestures 1.0

// Only works on platforms with Touch support.

Rectangle {
    id: rect
    width: 320
    height: 180

    Text {
        anchors.centerIn: parent
        text: "Tap / TapAndHold / Pan / Pinch / Swipe\nOnly works on platforms with Touch support."
        horizontalAlignment: Text.Center
    }

    GestureArea {
        anchors.fill: parent
        focus: true

        // Only some of the many gesture properties are shown. See Gesture documentation.

        onTap:
            console.log("tap pos = (",gesture.position.x,",",gesture.position.y,")")
        onTapAndHold:
            console.log("tap and hold pos = (",gesture.position.x,",",gesture.position.y,")")
        onPan:
            console.log("pan delta = (",gesture.delta.x,",",gesture.delta.y,") acceleration = ",gesture.acceleration)
        onPinch:
            console.log("pinch center = (",gesture.centerPoint.x,",",gesture.centerPoint.y,") rotation =",gesture.rotationAngle," scale =",gesture.scaleFactor)
        onSwipe:
            console.log("swipe angle=",gesture.swipeAngle)
        onGesture:
            console.log("gesture hot spot = (",gesture.hotSpot.x,",",gesture.hotSpot.y,")")
    }
}
