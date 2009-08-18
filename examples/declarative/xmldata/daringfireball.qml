import Qt 4.6

Rect {
    color: "white"
    width: 600
    height: 600
    resources: [
        XmlListModel {
            id: feedModel
            source: "http://daringfireball.net/index.xml"
            query: "/feed/entry"
            namespaceDeclarations: "declare default element namespace 'http://www.w3.org/2005/Atom';"
            XmlRole {
                name: "title"
                query: "title/string()"
            }
            XmlRole {
                name: "tagline"
                query: "author/name/string()"
            }
            XmlRole {
                name: "content"
                query: "content/string()"
            }
        },
        Component {
            id: feedDelegate
            Item {
                height: childrenRect.height + 20
                Text {
                    x: 10
                    id: TitleText
                    text: title
                    font.bold: true
                }
                Text {
                    text: 'by ' + tagline
                    anchors.left: TitleText.right
                    anchors.leftMargin: 10
                    font.italic: true
                }
                Text {
                    x: 10
                    text: content
                    anchors.top: TitleText.bottom
                    width: 580
                    wrap: true
                    onLinkActivated: { print('link clicked: ' + link) }
                }
            }
        }
    ]
    ListView {
        id: list
        anchors.fill: parent
        clip: true
        model: feedModel
        delegate: feedDelegate
    }
}
