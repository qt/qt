import Qt 4.7

Rectangle {
    id: root
    width:300; height:400

    property bool focus1: root.wantsFocus
    property bool focus2: item1.wantsFocus
    property bool focus3: fs1.wantsFocus
    property bool focus4: fs2.wantsFocus
    property bool focus5: theItem.wantsFocus

    Item {
        id: item1
        FocusScope {
            id: fs1
            focus: true
            FocusScope {
                id: fs2
                focus: true
                Item {
                    id: theItem
                    focus: true
                }
            }
        }
    }
}
