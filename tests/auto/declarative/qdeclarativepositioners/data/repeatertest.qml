import QtQuick 1.0

Item {
    width: 640
    height: 480
    Row {
        Repeater{ model: 3;
            delegate: Component {
                Rectangle {
                    color: "red"
                    width: 50
                    height: 50
                    z: {if(index == 0){2;}else if(index == 1){1;} else{3;}}
                    objectName: {if(index == 0){"one";}else if(index == 1){"two";} else{"three";}}

                }
            }
        }
    }
}
