import Qt 4.6

Rectangle {
    Component { id: TestableCursor
        //Doesn't blink
        Rectangle {
            color:"black"
            width:1
        }
    }
    color: "green"
    width:400;
    height:40;
    TextEdit {
        focus: true;
        cursorDelegate: TestableCursor
        text: "Jackdaws love my big sphinx of Quartz"
    }
}
