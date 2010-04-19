import Qt 4.7

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
                anchors { left: titleText.right; leftMargin: 10 }
                text: 'by ' + tagline
                font.italic: true
            }
            Text {
                x: 10
                width: 580
                anchors.top: titleText.bottom
                text: content
                wrapMode: Text.WordWrap

                onLinkActivated: { console.log('link clicked: ' + link) }
            }
        }
    }

    ListView {
        anchors.fill: parent
        model: feedModel
        delegate: feedDelegate
    }
}
