import Qt 4.7

Rectangle { width: 300; height: 300; color: "white"
    TextEdit {  text: "Hello world!"; id: textEditObject; objectName: "textEditObject"
        resources: [ Component { id:cursor; Item { id:cursorInstance; objectName: "cursorInstance" } } ] 
        cursorDelegate: cursor
    }
}
