import Qt 4.7

Rectangle {
    width: 200; height: 200

    Rectangle {
        width: 50; height: 50
        color: "red"

        Text { text: "Click"; anchors.centerIn: parent }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed: console.log('press (x: ' + mouse.x + ' y: ' + mouse.y + ' button: ' + (mouse.button == Qt.RightButton ? 'right' : 'left') + ' Shift: ' + (mouse.modifiers & Qt.ShiftModifier ? 'true' : 'false') + ')') 
            onReleased: console.log('release (x: ' + mouse.x + ' y: ' + mouse.y + ' isClick: ' + mouse.isClick + ' wasHeld: ' + mouse.wasHeld + ')') 
            onClicked: console.log('click (x: ' + mouse.x + ' y: ' + mouse.y + ' wasHeld: ' + mouse.wasHeld + ')')
            onDoubleClicked: console.log('double click (x: ' + mouse.x + ' y: ' + mouse.y + ')')
            onPressAndHold: console.log('press and hold')
            onEntered: console.log('entered ' + pressed)
            onExited: console.log('exited ' + pressed)
        }
    }

    Rectangle {
        y: 100; width: 50; height: 50
        color: "blue"

        Text { text: "Drag"; anchors.centerIn: parent }

        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: "XAxis"
            drag.minimumX: 0
            drag.maximumX: 150

            onPressed: console.log('press')
            onReleased: console.log('release (isClick: ' + mouse.isClick + ') (wasHeld: ' + mouse.wasHeld + ')')
            onClicked: console.log('click' + '(wasHeld: ' + mouse.wasHeld + ')')
            onDoubleClicked: console.log('double click')
            onPressAndHold: console.log('press and hold')
        }
    }
}
