import QtQuick 1.0

Rectangle { 
    QtObject {                                                                                    
        property real output: i1.p1 || i2.p2 == "text" ? 0.7  : 0
    }

    QtObject {
        id: i2
        property string p2
    }

    QtObject {
        id: i1
        property bool p1: false
    }
}  

