import Qt 4.7

Row { 
    Repeater { 
        objectName: "repeater"
        model: 5
        Text { 
            text: "I'm item " + index 
        } 
    }
}
