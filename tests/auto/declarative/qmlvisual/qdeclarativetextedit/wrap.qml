import QtQuick 1.0

Item {
    height:400
    width: 200
    TextEdit {
        width: 200
        height: 100
        wrapMode: TextEdit.WordWrap
        focus: true
    }
    //With QTBUG-6273 only the bottom one would be wrapped
    TextEdit {
        width: 200
        height: 100
        wrapMode: TextEdit.WordWrap
        text: "This is a test that text edit wraps correctly."
        y:100
    }
    TextEdit {
        width: 150
        height: 100
        wrapMode: TextEdit.WrapAnywhere
        text: "This is a test that text edit wraps correctly. thisisaverylongstringwithnospaces"
        y:200
    }
    TextEdit {
        width: 150
        height: 100
        wrapMode: TextEdit.Wrap
        text: "This is a test that text edit wraps correctly. thisisaverylongstringwithnospaces"
        y:300
    }
}
