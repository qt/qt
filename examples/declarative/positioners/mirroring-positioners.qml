import QtQuick 1.0

Rectangle {
    width: column.width+10
    height: column.height+10
    property bool arabic: false
    Column {
        id: column
        spacing: 10
        anchors.centerIn: parent
        Text {
            text: "Row"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Row {
            flow: arabic ? Row.RightToLeft : Row.LeftToRight
            spacing: 10
            Repeater {
                model: 4
                Loader {
                    property int value: index
                    sourceComponent: delegate
                }
            }
        }
        Text {
            text: "Grid"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Grid {
            flow: arabic ? Grid.RightToLeft : Grid.LeftToRight
            spacing: 10; columns: 4
            Repeater {
                model: 11
                Loader {
                    property int value: index
                    sourceComponent: delegate
                }
            }
        }
        Rectangle {
            height: 50; width: parent.width
            color: mouseArea.pressed ? "black" : "gray"
            Text {
                text: arabic ? "RTL" : "LTR"
                color: "white"
                font.pixelSize: 20
                anchors.centerIn: parent
            }
            MouseArea {
                id: mouseArea
                onClicked: arabic = !arabic
                anchors.fill: parent
            }
        }
    }
    Component {
        id: delegate
        Rectangle {
            width: 50; height: 50
            color: Qt.rgba(0.8/(parent.value+1),0.8/(parent.value+1),0.8/(parent.value+1),1.0)
            Text {
                text: parent.parent.value+1
                color: "white"
                font.pixelSize: 20
                anchors.centerIn: parent
            }
        }
    }
}
