import Qt.test 1.0

MyQmlContainer { 
    children : [ 
        MyQmlObject { 
            id: Object1 
            stringProperty: "hello" + Object2.stringProperty 
        },
        MyQmlObject { 
            id: Object2 
            stringProperty: "hello" + Object1.stringProperty 
        } 
    ] 
}
