import Qt 4.6

Rectangle {
    property string myText: "Lorem ipsum dolor sit amet, consectetur adipisicing elit"

    width: 800; height: 600
    color: Palette.base

    SystemPalette { id: Palette; colorGroup: Qt.Active }

    FontLoader { id: FixedFont; name: "Courier" }

    FontLoader { id: LocalFont; source: "fonts/Fontin-Bold.ttf" }
    /* A font by Jos Buivenga (exljbris) -> www.exljbris.nl */

    FontLoader { id: WebFont; source: "http://www.princexml.com/fonts/steffmann/Starburst.ttf" }
    FontLoader { id: WebFont2; source: "http://wrong.address.org" }

    VerticalPositioner {
        anchors.fill: parent
        anchors.leftMargin: 10; anchors.rightMargin: 10
        Text {
            text: myText
            color: Palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: "Times"
            font.pointSize: 32
        }
        Text {
            text: myText
            color: Palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: FixedFont.name
            font.pointSize: 32
        }
        Text {
            text: myText
            color: Palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: LocalFont.name
            font.pointSize: 32
        }
        Text {
            text: {
                if (WebFont.status == 1) myText
                else if (WebFont.status == 2) "Loading..."
                else if (WebFont.status == 3) "Error loading font"
            }
            color: Palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: WebFont.name
            font.pointSize: 32
        }
        Text {
            text: {
                if (WebFont2.status == 1) myText
                else if (WebFont2.status == 2) "Loading..."
                else if (WebFont2.status == 3) "Error loading font"
            }
            color: Palette.windowText
            width: parent.width; elide: "ElideRight"
            font.family: WebFont2.name
            font.pointSize: 32
        }
    }
}
