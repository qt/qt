import Qt 4.7

Rectangle {
    color: "white"
    width: 800
    height: 600

    Text { text: "All five rectangles should be red" }

    FocusScope {
        y: 100
        focus: true
        Rectangle { width: 50; height: 50; color: parent.activeFocus?"red":"blue" }

        FocusScope {
            y: 100
            focus: true
            Rectangle { width: 50; height: 50; color: parent.activeFocus?"red":"blue" }

            FocusScope {
                y: 100
                focus: true
                Rectangle { width: 50; height: 50; color: parent.activeFocus?"red":"blue" }

                FocusScope {
                    y: 100
                    focus: true
                    Rectangle { width: 50; height: 50; color: parent.activeFocus?"red":"blue" }

                    FocusScope {
                        y: 100
                        focus: true
                        Rectangle { width: 50; height: 50; color: parent.activeFocus?"red":"blue" }
                    }
                }
            }
        }
    }

}
