import Qt 4.6
import Qt.test 1.0

MyQmlObject {
    property alias c1: MyObject.c1
    property int c2: 3
    property int c3: c2 
    objectProperty: Object { 
        id: MyObject 
        property int c1
    }
}


