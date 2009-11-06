import Qt 4.6

Rectangle {
    width: childrenRect.width
    height: childrenRect.height
    Column {
        width: 80
        height: MyText.height*4
        Text {
            elide: "ElideLeft"
            text: "aaa bbb ccc ddd eee fff"
            width: 80
            id: MyText
        }
        Text {
            elide: "ElideMiddle"
            text: "aaa bbb ccc ddd eee fff"
            width: 80
        }
        Text {
            elide: "ElideRight"
            text: "aaa bbb ccc ddd eee fff"
            width: 80
        }
        Text {
            elide: "ElideNone"
            text: "aaa bbb ccc ddd eee fff"
            width: 80
        }
    }
}
