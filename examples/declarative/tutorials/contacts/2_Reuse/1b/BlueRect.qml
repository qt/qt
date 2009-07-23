import Qt 4.6

//! [all]
Rect {
    width: 100
    height: 100
    color: "blue"
    resources: [
        Component {
            id: redRectangle
            Rect {
                width: 30
                height: 30
                color: "red"
                radius: 5
            }
        }
    ]
    ComponentInstance {
        component: redRectangle
        anchors.right: parent.right
        anchors.top: parent.top
    }
    ComponentInstance {
        component: redRectangle
        anchors.left: parent.left
        anchors.top: parent.top
    }
    ComponentInstance {
        component: redRectangle
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}
//! [all]
