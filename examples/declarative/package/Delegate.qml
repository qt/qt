import Qt 4.7

//![0]
Package {
    Text { id: listDelegate; width: 200; height: 25; text: 'Empty'; Package.name: 'list' }
    Text { id: gridDelegate; width: 100; height: 50; text: 'Empty'; Package.name: 'grid' }

    Rectangle {
        id: wrapper
        width: 200; height: 25
        color: 'lightsteelblue'

        Text { text: display; anchors.centerIn: parent }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (wrapper.state == 'inList')
                    wrapper.state = 'inGrid';
                else
                    wrapper.state = 'inList';
            }
        }

        state: 'inList'
        states: [
            State {
                name: 'inList'
                ParentChange { target: wrapper; parent: listDelegate }
            },
            State {
                name: 'inGrid'
                ParentChange {
                    target: wrapper; parent: gridDelegate
                    x: 0; y: 0; width: gridDelegate.width; height: gridDelegate.height
                }
            }
        ]

        transitions: [
            Transition {
                ParentAnimation {
                    NumberAnimation { properties: 'x,y,width,height'; duration: 300 }
                }
            }
        ]
    }
}
//![0]
