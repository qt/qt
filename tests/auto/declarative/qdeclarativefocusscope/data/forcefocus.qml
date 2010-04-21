import Qt 4.7

Rectangle {
    width: 800; height: 600

    FocusScope {
        focus: true

        FocusScope {
            id: firstScope
            focus: true

            Rectangle {
                objectName: "item0"
                height: 120; width: 420

                color: "transparent"
                border.width: 5; border.color: firstScope.wantsFocus?"blue":"black"

                Rectangle {
                    id: item1; objectName: "item1"
                    x: 10; y: 10; width: 100; height: 100; color: "green"
                    border.width: 5; border.color: wantsFocus?"blue":"black"
                    focus: true

                    Rectangle {
                        width: 50; height: 50; anchors.centerIn: parent
                        color: parent.focus?"red":"transparent"
                    }
                }

                Rectangle {
                    id: item2; objectName: "item2"
                    x: 310; y: 10; width: 100; height: 100; color: "green"
                    border.width: 5; border.color: wantsFocus?"blue":"black"

                    Rectangle {
                        width: 50; height: 50; anchors.centerIn: parent
                        color: parent.focus?"red":"transparent"
                    }
                }
            }
        }

        FocusScope {
            id: secondScope

            Rectangle {
                objectName: "item3"
                y: 160; height: 120; width: 420

                color: "transparent"
                border.width: 5; border.color: secondScope.wantsFocus?"blue":"black"

                Rectangle {
                    id: item4; objectName: "item4"
                    x: 10; y: 10; width: 100; height: 100; color: "green"
                    border.width: 5; border.color: wantsFocus?"blue":"black"

                    Rectangle {
                        width: 50; height: 50; anchors.centerIn: parent
                        color: parent.focus?"red":"transparent"
                    }
                }

                Rectangle {
                    id: item5; objectName: "item5"
                    x: 310; y: 10; width: 100; height: 100; color: "green"
                    border.width: 5; border.color: wantsFocus?"blue":"black"

                    Rectangle {
                        width: 50; height: 50; anchors.centerIn: parent
                        color: parent.focus?"red":"transparent"
                    }
                }
            }
        }
    }
    Keys.onDigit4Pressed: item4.focus = true
    Keys.onDigit5Pressed: item5.forceFocus()
}
