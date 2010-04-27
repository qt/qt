import Qt 4.7

Rectangle { width: 300; height: 300; color: "white"
    TextInput {  text: "Hello world!"; id: textInputObject; objectName: "textInputObject"
        resources: [ Component { id:cursor; Item { id:cursorInstance; objectName: "cursorInstance";} } ] 
        cursorDelegate: cursor
    }
}
