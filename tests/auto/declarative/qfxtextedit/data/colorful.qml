import Qt 4.6

Rectangle {
    color:"lightsteelblue"
    width:600;
    height:100;
    Timer{ interval: 20; running: true; repeat: false; onTriggered: "Txt.selectionEnd = 14" }
    TextEdit {
        id: Txt
        focus: false;
        font.pointSize: 14
        font.bold: false
        color: "red"
        selectionColor: "yellow"
        selectedTextColor: "blue"
        text: "Jackdaws love my big sphinx of Quartz"
    }
}
