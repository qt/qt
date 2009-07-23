import Qt 4.6

Rect {
    width: contents.width
    height: contents.height
    VerticalLayout {
        width: 80
        height: Text.height*4
        Text {
            elide: "ElideLeft"
            text: "aaa bbb ccc ddd eee fff"
            width: 80
            color: "white"
            id: Text
        }
        Text {
            elide: "ElideMiddle"
            text: "aaa bbb ccc ddd eee fff"
            width: 80
            color: "white"
        }
        Text {
            elide: "ElideRight"
            text: "aaa bbb ccc ddd eee fff"
            width: 80
            color: "white"
        }
        Text {
            elide: "ElideNone"
            text: "aaa bbb ccc ddd eee fff"
            width: 80
            color: "white"
        }
    }
}
