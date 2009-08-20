import Qt 4.6

Rectangle {
    id: MainWindow;
    width: 320; height: 270; color: Palette.window

    Palette { id: Palette }
    Script { source: "calculator.js" }

    VerticalPositioner {
        x: 2; spacing: 10;

        Rectangle {
            id: Container
            width: 316; height: 50; z: 2
            border.color: Palette.dark; color: Palette.base

            Text {
                id: CurNum
                font.bold: true; font.pointSize: 16
                color: Palette.text
                anchors.right: Container.right
                anchors.rightMargin: 5
                anchors.verticalCenter: Container.verticalCenter
            }

            Text {
                id: CurrentOperation
                color: Palette.text
                font.bold: true; font.pointSize: 16
                anchors.left: Container.left
                anchors.leftMargin: 5
                anchors.verticalCenter: Container.verticalCenter
            }
        }

        Item {
            width: 320; height: 30

            CalcButton {
                id: AdvancedCheckBox
                x: 55; width: 206
                operation: "Advanced Mode"
                toggable: true
            }
        }

        Item {
            width: 320

            Item {
                id: BasicButtons
                x: 55; width: 160; height: 160

                CalcButton { operation: "Bksp"; id: Bksp; width: 67; opacity: 0 }
                CalcButton { operation: "C"; id: C; width: 76 }
                CalcButton { operation: "AC"; id: AC; x: 78; width: 76 }

                GridPositioner {
                    id: NumKeypad; y: 32; spacing: 2; columns: 3

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

                HorizontalPositioner {
                    y: 128; spacing: 2

                    CalcButton { operation: "0"; width: 50 }
                    CalcButton { operation: "."; x: 77; width: 50 }
                    CalcButton { operation: "="; id: Equals; x: 77;  width: 102 }
                }

                VerticalPositioner {
                    id: SimpleOperations
                    x: 156; y: 0; spacing: 2

                    CalcButton { operation: "x" }
                    CalcButton { operation: "/" }
                    CalcButton { operation: "-" }
                    CalcButton { operation: "+" }
                }
            }

            GridPositioner {
                id: AdvancedButtons
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

    states: [
        State {
            name: "Advanced"; when: AdvancedCheckBox.toggled == true
            SetProperties { target: BasicButtons; x: 0 }
            SetProperties { target: SimpleOperations; y: 32 }
            SetProperties { target: Bksp; opacity: 1 }
            SetProperties { target: C; x: 69; width: 67 }
            SetProperties { target: AC; x: 138; width: 67 }
            SetProperties { target: Equals; width: 50 }
            SetProperties { target: AdvancedButtons; x: 210; opacity: 1 }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation { properties: "x,y,width"; easing: "easeOutBounce"; duration: 500 }
            NumberAnimation { properties: "opacity"; easing: "easeInOutQuad"; duration: 500 }
        }
    ]
}
