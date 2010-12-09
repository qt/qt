import QtQuick 1.0

Rectangle {
    width: 200; height: 200

  //  Column {
        FocusScope {
            x: rect1.x; y:rect1.y; width: rect1.width; height: rect1.height
            Rectangle {id: rect1; width: 50; height: 50; focus:true
                   color: focus ? "red":"blue"
            }
            Rectangle {id: rect2; width: 50; height: 50; focus:true
                   color: focus ? "red":"blue"
                    y: 75
            }
//        }
/*
        FocusScope {
            x: rect2.x; y:rect2.y; width: rect2.width; height: rect2.height
            Rectangle {id: rect2; width: 50; height: 50; color: "red"}
       }
*/
    }

}
