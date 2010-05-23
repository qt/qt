import Qt 4.7

Rectangle {
    width: 600; height: 300; color: "white"

    ListModel {
        id: myModel
        ListElement {
            itemColor: "red"
        }
        ListElement {
            itemColor: "green"
        }
        ListElement {
            itemColor: "blue"
        }
        ListElement {
            itemColor: "orange"
        }
        ListElement {
            itemColor: "brown"
        }
        ListElement {
            itemColor: "yellow"
        }
        ListElement {
            itemColor: "purple"
        }
        ListElement {
            itemColor: "darkred"
        }
        ListElement {
            itemColor: "darkblue"
        }
    }

    Component {
        id: myDelegate
        Item {
            width: 200; height: 50
            Rectangle {
                x: 5; y : 5
                width: 190; height: 40
                opacity: 0.5
                color: itemColor
            }
        }
    }

    Component {
        id: myHighlight
        Rectangle { color: "black" }
    }

    ListView {
        id: list1
        width: 200; height: parent.height
        model: myModel; delegate: myDelegate
        highlight: myHighlight; currentIndex: list3.currentIndex
        focus: true
    }
    ListView {
        id: list2
        x: 200; width: 200; height: parent.height
        model: myModel; delegate: myDelegate; highlight: myHighlight
        preferredHighlightBegin: 80
        preferredHighlightEnd: 220
        highlightRangeMode: "ApplyRange"
        currentIndex: list1.currentIndex
    }
    ListView {
        id: list3
        x: 400; width: 200; height: parent.height
        model: myModel; delegate: myDelegate; highlight: myHighlight
        currentIndex: list1.currentIndex
        preferredHighlightBegin: 125
        preferredHighlightEnd: 125
        highlightRangeMode: "StrictlyEnforceRange"
        flickDeceleration: 1000
    }
}
