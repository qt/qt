import Qt 4.6

Rectangle {
    width: 320; height: 270; color: palette.window

    SystemPalette { id: palette }
    Script { source: "calculator.js" }

    Column {
        x: 2; spacing: 10;

        Rectangle {
            id: container
            width: 316; height: 50
            border.color: palette.dark; color: palette.base

            Text {
                id: curNum
                font.bold: true; font.pointSize: 16
                color: palette.text
                anchors.right: container.right
                anchors.rightMargin: 5
                anchors.verticalCenter: container.verticalCenter
            }

            Text {
                id: currentOperation
                color: palette.text
                font.bold: true; font.pointSize: 16
                anchors.left: container.left
                anchors.leftMargin: 5
                anchors.verticalCenter: container.verticalCenter
            }
        }

        Item {
            width: 320; height: 30

            CalcButton {
                id: advancedCheckBox
                x: 55; width: 206
                operation: "Advanced Mode"
                toggable: true
            }
        }

        Item {
            width: 320; height: 160

            Item {
                id: basicButtons
                x: 55; width: 160; height: 160

                CalcButton { operation: "Bksp"; id: bksp; width: 67; opacity: 0 }
                CalcButton { operation: "C"; id: c; width: 76 }
                CalcButton { operation: "AC"; id: ac; x: 78; width: 76 }

                Grid {
                    id: numKeypad; y: 32; spacing: 2; columns: 3

                    CalcButton { operation: "7" }
                    CalcButton { operation: "8" }
                    CalcButton { operation: "9" }
                    CalcButton { operation: "4" }
                    CalcButton { operation: "5" }
                    CalcButton { operation: "6" }
                    CalcButton { operation: "1" }
                    CalcButton { operation: "2" }
                    CalcButton { operation: "3" }
                }

                Row {
                    y: 128; spacing: 2

                    CalcButton { operation: "0"; width: 50 }
                    CalcButton { operation: "."; x: 77; width: 50 }
                    CalcButton { operation: "="; id: equals; x: 77;  width: 102 }
                }

                Column {
                    id: simpleOperations
                    x: 156; y: 0; spacing: 2

                    CalcButton { operation: "x" }
                    CalcButton { operation: "/" }
                    CalcButton { operation: "-" }
                    CalcButton { operation: "+" }
                }
            }

            Grid {
                id: advancedButtons
                x: 350; spacing: 2; columns: 2; opacity: 0

                CalcButton { operation: "Abs" }
                CalcButton { operation: "Int" }
                CalcButton { operation: "MC" }
                CalcButton { operation: "Sqrt" }
                CalcButton { operation: "MR" }
                CalcButton { operation: "^2" }
                CalcButton { operation: "MS" }
                CalcButton { operation: "1/x" }
                CalcButton { operation: "M+" }
                CalcButton { operation: "+/-" }
            }
        }
    }

    states: State {
        name: "Advanced"; when: advancedCheckBox.toggled == true
        PropertyChanges { target: basicButtons; x: 0 }
        PropertyChanges { target: simpleOperations; y: 32 }
        PropertyChanges { target: bksp; opacity: 1 }
        PropertyChanges { target: c; x: 69; width: 67 }
        PropertyChanges { target: ac; x: 138; width: 67 }
        PropertyChanges { target: equals; width: 50 }
        PropertyChanges { target: advancedButtons; x: 210; opacity: 1 }
    }

    transitions: Transition {
        NumberAnimation { properties: "x,y,width"; easing.type: "OutBounce"; duration: 500 }
        NumberAnimation { properties: "opacity"; easing.type: "InOutQuad"; duration: 500 }
    }
}
