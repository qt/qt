import Qt 4.6

Rectangle {
    gradient: Gradient {
        GradientStop { position: 0; color: "black" }
        GradientStop { position: 1.0; color: "#AAAAAA" }
    }
    width: 600
    height: 600
    resources: [
        XmlListModel {
            id: feedModel
            source: "http://rss.news.yahoo.com/rss/oceania"
            query: "/rss/channel/item"
            XmlRole {
                name: "title"
                query: "title/string()"
            }
            XmlRole {
                name: "link"
                query: "link/string()"
            }
            XmlRole {
                name: "description"
                query: "description/string()"
            }
        },
        Component {
            id: feedDelegate
            Item {
                id: delegate
                height: wrapper.height + 10
                MouseRegion {
                    anchors.fill: wrapper
                    onPressed: { delegate.ListView.view.currentIndex = index; }
                    onClicked: { if (wrapper.state == 'Details') { wrapper.state = '';} else {wrapper.state = 'Details';} }
                }
                Rectangle {
                    id: wrapper
                    y: 5
                    height: titleText.height + 10
                    width: 580
                    color: "#F0F0F0"
                    radius: 5
                    Text {
                        x: 10
                        y: 5
                        id: titleText
                        text: '<a href=\'' + link + '\'>' + title + '</a>'
                        font.bold: true
                        font.family: "Helvetica"
                        font.pointSize: 14
                        onLinkActivated: { print('link clicked: ' + link) }
                    }
                    Text {
                        x: 10
                        id: description
                        text: description
                        width: 560
                        wrap: true
                        font.family: "Helvetica"
                        anchors.top: titleText.bottom
                        anchors.topMargin: 5
                        opacity: 0
                    }
                    states: [
                        State {
                            name: "Details"
                            PropertyChanges { target: wrapper; height: childrenRect.height + 10 }
                            PropertyChanges { target: description; opacity: 1 }
                        }
                    ]
                    transitions: [
                        Transition {
                            from: "*"
                            to: "Details"
                            reversible: true
                            SequentialAnimation {
                                NumberAnimation {
                                    duration: 200
                                    properties: "height"
                                    easing: "easeOutQuad"
                                }
                                NumberAnimation {
                                    duration: 200
                                    properties: "opacity"
                                }
                            }
                        }
                    ]
                }
            }
        }
    ]
    ListView {
        id: list
        x: 10
        y: 10
        width: parent.width - 20
        height: parent.height - 20
        clip: true
        model: feedModel
        delegate: feedDelegate
    }
}
