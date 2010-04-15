import Qt 4.7
import "Core"
import "Core/calculator.js" as CalcEngine

Rectangle {
    id: window

    width: 480; height: 360
    color: "#282828"

    property string rotateLeft: "\u2939"
    property string rotateRight: "\u2935"
    property string leftArrow: "\u2190"
    property string division : "\u00f7"
    property string multiplication : "\u00d7"
    property string squareRoot : "\u221a"
    property string plusminus : "\u00b1"

    function doOp(operation) { CalcEngine.doOperation(operation) }

    Item {
        id: main
        state: (runtime.orientation == Orientation.Portrait) ? '' : 'rotated'
        width: parent.width; height: parent.height; anchors.centerIn: parent

        Column {
            id: box; spacing: 8

            anchors { fill: parent; topMargin: 6; bottomMargin: 6; leftMargin: 6; rightMargin: 6 }

            Row {
                Display { id: display; width: box.width; height: 64 }
            }

            Column {
                id: column; spacing: 6

                property real h: ((box.height - 72) / 6) - ((spacing * (6 - 1)) / 6)
                property real w: (box.width / 4) - ((spacing * (4 - 1)) / 4)

                Row {
                    spacing: 6

                    Button {
                        id: rotateButton
                        width: column.w; height: column.h; color: 'purple'; operation: rotateLeft
                    }
                    Button { width: column.w; height: column.h; color: 'purple'; operation: leftArrow }
                    Button { width: column.w; height: column.h; color: 'purple'; operation: "C" }
                    Button { width: column.w; height: column.h; color: 'purple'; operation: "AC" }
                }

                Row {
                    spacing: 6
                    property real w: (box.width / 4) - ((spacing * (4 - 1)) / 4)

                    Button { width: column.w; height: column.h; color: 'green'; operation: "mc" }
                    Button { width: column.w; height: column.h; color: 'green'; operation: "m+" }
                    Button { width: column.w; height: column.h; color: 'green'; operation: "m-" }
                    Button { width: column.w; height: column.h; color: 'green'; operation: "mr" }
                }

                Grid {
                    id: grid; rows: 4; columns: 5; spacing: 6

                    property real w: (box.width / columns) - ((spacing * (columns - 1)) / columns)

                    Button { width: grid.w; height: column.h; operation: "7"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "8"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "9"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: division }
                    Button { width: grid.w; height: column.h; operation: squareRoot }
                    Button { width: grid.w; height: column.h; operation: "4"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "5"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "6"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: multiplication }
                    Button { width: grid.w; height: column.h; operation: "x^2" }
                    Button { width: grid.w; height: column.h; operation: "1"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "2"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "3"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "-" }
                    Button { width: grid.w; height: column.h; operation: "1/x" }
                    Button { width: grid.w; height: column.h; operation: "0"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "." }
                    Button { width: grid.w; height: column.h; operation: plusminus }
                    Button { width: grid.w; height: column.h; operation: "+" }
                    Button { width: grid.w; height: column.h; operation: "="; color: 'red' }
                }
            }
        }

        states: State {
            name: 'rotated'
            PropertyChanges { target: main; rotation: -90; width: window.height; height: window.width }
            PropertyChanges { target: rotateButton; operation: rotateRight }
        }

        transitions: Transition {
            SequentialAnimation {
                PropertyAction { target: rotateButton; property: "operation" }
                NumberAnimation { properties: "rotation"; duration: 300; easing.type: "InOutQuint" }
                NumberAnimation { properties: "x,y,width,height"; duration: 300; easing.type: "InOutQuint" }
            }
        }
    }
}
