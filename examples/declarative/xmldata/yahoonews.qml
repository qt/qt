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
                id: Delegate
                height: Wrapper.height + 10
                MouseRegion {
                    anchors.fill: Wrapper
                    onPressed: { Delegate.ListView.list.currentIndex = index; }
                    onClicked: { if (Wrapper.state == 'Details') { Wrapper.state = '';} else {Wrapper.state = 'Details';} }
                }
                Rectangle {
                    id: Wrapper
                    y: 5
                    height: TitleText.height + 10
                    width: 580
                    color: "#F0F0F0"
                    radius: 5
                    Text {
                        x: 10
                        y: 5
                        id: TitleText
                        text: '<a href=\'' + link + '\'>' + title + '</a>'
                        font.bold: true
                        font.family: "Helvetica"
                        font.pointSize: 14
                        onLinkActivated: { print('link clicked: ' + link) }
                    }
                    Text {
                        x: 10
                        id: Description
                        text: description
                        width: 560
                        wrap: true
                        font.family: "Helvetica"
                        anchors.top: TitleText.bottom
                        anchors.topMargin: 5
                        opacity: 0
                    }
                    states: [
                        State {
                            name: "Details"
                            PropertyChanges { target: Wrapper; height: childrenRect.height + 10 }
                            PropertyChanges { target: Description; opacity: 1 } 
                        }
                    ]
                    transitions: [
                        Transition {
                            fromState: "*"
                            toState: "Details"
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
        currentItemPositioning: "Snap"
    }
}
