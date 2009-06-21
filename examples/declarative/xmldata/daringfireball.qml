Rect {
    color: "white"
    width: 600
    height: 600
    resources: [
        XmlListModel {
            id: feedModel
            source: "http://daringfireball.net/index.xml"
            query: "doc($src)/feed/entry"
            namespaceDeclarations: "declare default element namespace 'http://www.w3.org/2005/Atom';"
            Role {
                name: "title"
                query: "title/string()"
            }
            Role {
                name: "tagline"
                query: "author/name/string()"
            }
            Role {
                name: "content"
                query: "content/string()"
            }
        },
        Component {
            id: feedDelegate
            Item {
                height: contents.height + 20
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
