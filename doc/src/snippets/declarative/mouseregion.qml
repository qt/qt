import Qt 4.6

Rect { width: 200; height: 100
HorizontalLayout {
//! [0]
Rect { width: 100; height: 100; color: "green"
    MouseRegion { anchors.fill: parent; onClicked: { parent.color = 'red' } }
}
//! [0]
//! [1]
Rect {
    width: 100; height: 100; color: "green"
    MouseRegion {
        anchors.fill: parent
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
