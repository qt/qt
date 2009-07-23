import Qt 4.6

Item {
    width: 320
    height: 480
    Rect {
        color: "blue"
        x: 20
        y: 20
        width: 20
        height: 20
        Rect {
            color: "black"
            x: 20
            y: 20
            width: 10
            height: 10
        }
    }
    Rect {
        color: "red"
        x: 40
        y: 20
        width: 20
        height: 20
    }
    Rect {
        color: "green"
        x: 60
        y: 20
        width: 20
        height: 20
    }
    Rect {
        color: "yellow"
        x: 20
        y: 40
        width: 20
        height: 20
    }
    Rect {
        color: "purple"
        x: 20
        y: 60
        width: 20
        height: 20
    }
    Rect {
        color: "white"
        x: 40
        y: 40
        width: 20
        height: 20
    }
    Rect {
        anchors.fill: parent
        color: "gray"
        z: -1
    }
}
