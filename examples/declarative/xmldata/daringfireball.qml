import Qt 4.6

Rectangle {
    color: "white"
    width: 600; height: 600

    XmlListModel {
        id: feedModel
        source: "http://daringfireball.net/index.xml"
        query: "/feed/entry"
        namespaceDeclarations: "declare default element namespace 'http://www.w3.org/2005/Atom';"
        XmlRole { name: "title"; query: "title/string()" }
        XmlRole { name: "tagline"; query: "author/name/string()" }
        XmlRole { name: "content"; query: "content/string()" }
    }

    Component {
        id: feedDelegate
        Item {
            height: childrenRect.height + 20
            Text {
                id: titleText
                x: 10
                text: title; font.bold: true
            }
            Text {
                text: 'by ' + tagline
                anchors.left: titleText.right; anchors.leftMargin: 10
                font.italic: true
            }
            Text {
                x: 10
                text: content
                anchors.top: titleText.bottom
                width: 580; wrap: true
                onLinkActivated: { console.log('link clicked: ' + link) }
            }
        }
    }

    ListView {
        anchors.fill: parent
        model: feedModel; delegate: feedDelegate
    }
}
