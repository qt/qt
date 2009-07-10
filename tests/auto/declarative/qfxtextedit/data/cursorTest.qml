Rect { width: 300; height: 300; color: "white"
    TextEdit {  text: "Hello world!"; focusable: true; id: textEditObject
        resources: [ Component { id:cursor; Item { id:cursorInstance } } ] 
        cursorDelegate: cursor
    }
}
