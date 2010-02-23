//![0]
import Qt 4.6

Rectangle {
    id: page
    width: 500; height: 200
    color: "lightgray"

    Text {
        id: helloText
        text: "Hello world!"
        font.pointSize: 24; font.bold: true
        y: 30; anchors.horizontalCenter: page.horizontalCenter
        transformOrigin: Item.Center

//![1]
        MouseArea { id: mouseRegion; anchors.fill: parent }
//![1]

//![2]
        states: State {
            name: "down"; when: mouseRegion.pressed == true
            PropertyChanges { target: helloText; y: 160; rotation: 180; color: "red" }
        }
//![2]

//![3]
        transitions: Transition {
            from: ""; to: "down"; reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y,rotation"; duration: 500; easing: "easeInOutQuad" }
                ColorAnimation { duration: 500 }
            }
        }
//![3]
    }

    Grid {
        id: colorPicker
        anchors.bottom: page.bottom
        rows: 2; columns: 3; spacing: 3

        Cell { color: "red"; onClicked: helloText.color = color }
        Cell { color: "green"; onClicked: helloText.color = color }
        Cell { color: "blue"; onClicked: helloText.color = color }
        Cell { color: "yellow"; onClicked: helloText.color = color }
        Cell { color: "steelblue"; onClicked: helloText.color = color }
        Cell { color: "black"; onClicked: helloText.color = color }
    }
}
//![0]
