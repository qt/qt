import Qt 4.6

Item {
    height:400
    width: 200
    TextEdit {
        width: 200
        height: 200
        wrap: true
        focus: true
    }
    //With QTBUG-6273 only the bottom one would be wrapped
    TextEdit {
        width: 200
        height: 200
        wrap: true
        text: "This is a test that text edit wraps correctly."
        y:200
    }

}
