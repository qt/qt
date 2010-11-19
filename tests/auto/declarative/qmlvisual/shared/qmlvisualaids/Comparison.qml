import Qt 4.7

Item{
    width: 1000
    height:800
    Row{
        spacing: 4
        Button{
            caption: "run"
            onTriggered: test.runTest()
        }

        Button{
            caption: "update"
            onTriggered: test.updateVisuals()
        }

        Button{
            caption: "platform"
            onTriggered: test.updatePlatformVisuals()
        }
    }

    Rectangle {
        y: 180
        width: 1000
        height: 620
        Row{
            id: grid
            spacing: 4
            Text{
                width: 300
                height: 200
                text: test.testName
                clip: true; wrapMode: Text.WordWrap
            }
            Text{
                width: 300
                height: 200
                text: test.testCase
                clip: true; wrapMode: Text.WordWrap
            }
            Text{
                width: 300
                height: 200
                text:  test.testScript
                clip: true; wrapMode: Text.WordWrap
            }
        }
        Item{
            y: 200
            Row{
                ListView{
                    width: 200; height: 400
                    delegate: Rectangle{ width: 200; height: 200; color: "blue"; Image{ source: modelData }}
                    model:  test.goodImages;
                }
                ListView{
                    width: 200; height: 400
                    delegate: Rectangle{ width: 200; height: 200; color: "blue"; Image{ source: modelData }}
                    model:  test.diffImages;
                }
                ListView{
                    width: 200; height: 400
                    delegate: Rectangle{ width: 200; height: 200; color: "blue"; Image{ source: modelData }}
                    model:  test.badImages;
                }
            }
        }
        MouseArea{
            anchors.fill: parent
            onClicked: test.moveOn();
        }
    }
}
