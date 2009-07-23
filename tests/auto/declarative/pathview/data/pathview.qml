import Qt 4.6

Rect {
    width: 240
    height: 320
    color: "#ffffff"
    resources: [
        Component {
            id: Delegate
            Rect {
                id: wrapper
                height: 20
                width: 60
                color: "white"
                pen.color: "black"
                Text {
                    text: index
                }
                Text {
                    x: 20
                    id: textName
                    text: name
                }
                Text {
                    x: 40
                    id: textNumber
                    text: number
                }
            }
        }
    ]
    PathView {
        id: view
        width: 240
        height: 320
        model: testModel
        delegate: Delegate
        snapPos: 0.01
        path: Path {
            startY: 120
            startX: 160
            PathQuad {
                y: 120
                x: 80
                controlY: 330
                controlX: 100
            }
            PathLine {
                y: 160
                x: 20
            }
            PathCubic {
                y: 120
                x: 160
                control1Y: 0
                control1X: 100
                control2Y: 000
                control2X: 200
            }
        }
    }
}
