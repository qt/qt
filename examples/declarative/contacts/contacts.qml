Rect {
    id: page
    width: 320
    height: 480
    color: "white"
    resources: [
        Component {
            id: contactDelegate
            Rect {
                id: wrapper
                x: 20
                width: List.width-40
                color: "#FEFFEE"
                pen.color: "#FFBE4F"
                radius: 5
                filter: Shadow {
                    xOffset: 5
                    yOffset: 5
                }
                MouseRegion {
                    id: pageMouse
                    anchors.fill: parent
                    onClicked: { if (wrapper.state == 'Details') { wrapper.state = '';} else {wrapper.state = 'Details';} }
                }
                Image {
                    id: portraitPic
                    source: portrait
                    x: 10
                    y: 10
                }
                Text {
                    id: name
                    text: firstName + ' ' + lastName
                    anchors.left: portraitPic.right
                    anchors.leftMargin: 10
                    anchors.top: portraitPic.top
                    anchors.right: wrapper.right
                    anchors.rightMargin: 10
                    font.family: "Comic Sans MS"
                    font.bold: true
                    font.size: 11
                }
                VerticalLayout {
                    id: email_layout
                    anchors.left: name.left
                    anchors.top: name.bottom
                    anchors.topMargin: 10
                    Repeater {
                        id: email_list
                        dataSource: emails
                        Component {
                            Text {
                                text: modelData
                                height: 18
                                font.italic: true
                                color: "midnightblue"
                            }
                        }
                    }
                }
                height: Math.max(email_layout.height + name.height + 25, portraitPic.height+20)
                states: [
                    State {
                        name: "Details"
                        SetProperty {
                            target: wrapper
                            property: "color"
                            value: "white"
                        }
                        SetProperty {
                            target: wrapper
                            property: "x"
                            value: 0
                        }
                        SetProperty {
                            target: wrapper
                            property: "height"
                            value: List.height
                        }
                        SetProperty {
                            target: wrapper
                            property: "width"
                            value: List.width
                        }
                        SetProperty {
                            target: wrapper.ListView.view
                            property: "yPosition"
                            value: wrapper.y
                        }
                        SetProperty {
                            target: wrapper.ListView.view
                            property: "locked"
                            value: 1
                        }
                    }
                ]
                transitions: [
                    Transition {
                        ParallelAnimation {
                            ColorAnimation {
                                duration: 500
                            }
                            NumericAnimation {
                                duration: 150
                                properties: "x,yPosition,height,width"
                            }
                        }
                    }
                ]
            }
        }
    ]
    ListView {
        id: List
        model: contactModel
        width: 320
        height: 480
        clip: true
        delegate: contactDelegate
    }
}
