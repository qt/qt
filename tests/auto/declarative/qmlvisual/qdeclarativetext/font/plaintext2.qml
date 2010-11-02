import QtQuick 1.0

Rectangle {
    width: 400; height: 200

    Row {
        spacing: 20
        anchors.centerIn: parent
        Text {
            text: "First line\nSecond line"; wrapMode: Text.Wrap
        }
        Text {
            text: "First line\nSecond line"; width: 70
        }
        Text {
            text: "First Second\nThird Fourth"; wrapMode: Text.Wrap; width: 50
        }
        Text {
            text: "First line<br>Second line"; textFormat: Text.StyledText
        }
    }
}
