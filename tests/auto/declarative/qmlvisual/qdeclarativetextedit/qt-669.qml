import QtQuick 1.0

Rectangle {
    Component { id: testableCursor
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
        cursorDelegate: testableCursor
        text: "Jackdaws love my big sphinx of Quartz"
    }
}
