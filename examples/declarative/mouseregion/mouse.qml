import Qt 4.6

Rectangle {
    color: "white"
    width: 200; height: 200
    Rectangle {
        width: 50; height: 50
        color: "red"
        Text { text: "Click"; anchors.centerIn: parent }
        MouseRegion {
            hoverEnabled: true
            onPressed: { print('press (x: ' + mouse.x + ' y: ' + mouse.y + ' button: ' + (mouse.button == Qt.RightButton ? 'right' : 'left') + ' Shift: ' + (mouse.modifiers & Qt.ShiftModifier ? 'true' : 'false') + ')') }
            onReleased: { print('release (x: ' + mouse.x + ' y: ' + mouse.y + ' isClick: ' + mouse.isClick + ' wasHeld: ' + mouse.wasHeld + ')') }
            onClicked: { print('click (x: ' + mouse.x + ' y: ' + mouse.y + ' wasHeld: ' + mouse.wasHeld + ')') }
            onDoubleClicked: { print('double click (x: ' + mouse.x + ' y: ' + mouse.y + ')') }
            onPressAndHold: { print('press and hold') }
            onEntered: { print('entered ' + pressed) }
            onExited: { print('exited ' + pressed) }
            anchors.fill: parent
        }
    }
    Rectangle {
        y: 100; width: 50; height: 50
        color: "blue"
        Text { text: "Drag"; anchors.centerIn: parent }
        MouseRegion {
            drag.target: parent
            drag.axis: "XAxis"
            drag.minimumX: 0
            drag.maximumX: 150
            onPressed: { print('press') }
            onReleased: { print('release (isClick: ' + mouse.isClick + ') (wasHeld: ' + mouse.wasHeld + ')') }
            onClicked: { print('click' + '(wasHeld: ' + mouse.wasHeld + ')') }
            onDoubleClicked: { print('double click') }
            onPressAndHold: { print('press and hold') }
            anchors.fill: parent
        }
    }
}
