import Qt 4.7

Rectangle {
    id: s; width: 800; height: 1000; color: "lightsteelblue"
    property string text: "<b>The</b> <i>quick</i> <u>brown</u> <o>fox</o> <big>jumps</big> <small>over</small> <tt>the</tt> <s>lazy</s> <em>dog</em>."

    Column {
        spacing: 10
        Text {
            text: s.text
        }
        Text {
            text: s.text; font.pixelSize: 18
        }
        Text {
            text: s.text; font.pointSize: 25
        }
        Text {
            text: s.text; color: "red"; smooth: true
        }
        Text {
            text: s.text; font.capitalization: "AllUppercase"
        }
        Text {
            text: s.text; font.underline: true
        }
        Text {
            text: s.text; font.overline: true; smooth: true
        }
        Text {
            text: s.text; font.strikeout: true
        }
        Text {
            text: s.text; font.underline: true; font.overline: true; font.strikeout: true
        }
        Text {
            text: s.text; font.letterSpacing: 2
        }
        Text {
            text: s.text; font.underline: true; font.letterSpacing: 2; font.capitalization: "AllUppercase"; color: "blue"
        }
        Text {
            text: s.text; font.overline: true; font.wordSpacing: 25; font.capitalization: "Capitalize"; color: "green"
        }
        Text {
            text: s.text; font.pixelSize: 18; style: Text.Outline; styleColor: "white"
        }
        Text {
            text: s.text; font.pixelSize: 18; style: Text.Sunken; styleColor: "gray"
        }
        Text {
            text: s.text; font.pixelSize: 18; style: Text.Raised; styleColor: "yellow"
        }
        Text {
            text: s.text; horizontalAlignment: Text.AlignLeft; width: 800
        }
        Text {
            text: s.text; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; width: 800; height: 20
        }
        Text {
            text: s.text; horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignBottom; width: 800; height: 20
        }
        Text {
            text: s.text; font.pixelSize: 18; style: Text.Outline; styleColor: "white"; wrapMode: Text.WordWrap; width: 200
        }
        Text {
            text: s.text; elide: Text.ElideLeft; width: 200
        }
        Text {
            text: s.text; elide: Text.ElideMiddle; width: 200
        }
        Text {
            text: s.text; elide: Text.ElideRight; width: 200
        }
        Text {
            text: s.text; elide: Text.ElideLeft; width: 200; wrapMode: Text.WordWrap
        }
        Text {
            text: s.text; elide: Text.ElideMiddle; width: 200; wrapMode: Text.WordWrap
        }
        Text {
            text: s.text; elide: Text.ElideRight; width: 200; wrapMode: Text.WordWrap
        }
        Text {
            text: s.text + " thisisaverylongstringwithnospaces"; width: 150; wrapMode: Text.WrapAnywhere
        }
        Text {
            text: s.text + " thisisaverylongstringwithnospaces"; width: 150; wrapMode: Text.Wrap
        }
    }
}
