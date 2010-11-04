import QtQuick 1.0
import "../shared" 1.0

TextEdit {
    id: edit
    FontLoader { id: fixedFont; source: "Vera.ttf" }
    font.family: fixedFont.name
    font.pixelSize: 12
    cursorDelegate: Rectangle {
            width: 1;
            color: "black";
            visible: edit.cursorVisible
    }
}
