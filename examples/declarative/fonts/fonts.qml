import Qt 4.6

Rectangle {
    property string myText: "Lorem ipsum dolor sit amet, consectetur adipisicing elit"

    width: 800; height: 600
    color: palette.base

    SystemPalette { id: palette; colorGroup: Qt.Active }

    FontLoader { id: fixedFont; name: "Courier" }

    FontLoader { id: localFont; source: "fonts/Fontin-Bold.ttf" }
    /* A font by Jos Buivenga (exljbris) -> www.exljbris.nl */

    FontLoader { id: webFont; source: "http://www.princexml.com/fonts/steffmann/Starburst.ttf" }
    FontLoader { id: webFont2; source: "http://wrong.address.org" }

    Column {
        anchors.fill: parent
        anchors.leftMargin: 10; anchors.rightMargin: 10
        Text {
            text: myText
            color: palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: "Times"
            font.pointSize: 32
        }
        Text {
            text: myText
            color: palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: fixedFont.name
            font.pointSize: 32
        }
        Text {
            text: myText
            color: palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: localFont.name
            font.pointSize: 32
        }
        Text {
            text: {
                if (webFont.status == 1) myText
                else if (webFont.status == 2) "Loading..."
                else if (webFont.status == 3) "Error loading font"
            }
            color: palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: webFont.name
            font.pointSize: 32
        }
        Text {
            text: {
                if (webFont2.status == 1) myText
                else if (webFont2.status == 2) "Loading..."
                else if (webFont2.status == 3) "Error loading font"
            }
            color: palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: webFont2.name
            font.pointSize: 32
        }
    }
}
