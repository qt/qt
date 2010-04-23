import Qt 4.7

Rectangle { width: 200; height: 100
Row {
//! [0]
Rectangle { width: 100; height: 100; color: "green"
    MouseArea { anchors.fill: parent; onClicked: { parent.color = 'red' } }
}
//! [0]
//! [1]
Rectangle {
    width: 100; height: 100; color: "green"
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button == Qt.RightButton)
                parent.color = 'blue';
            else
                parent.color = 'red';
        }
    }
}
//! [1]
}
}
