import Qt 4.7

//![0]
Rectangle {
     color: "lightsteelblue"; width: 240; height: 320

     ListView {
         anchors.fill: parent
         focus: true

         model: ListModel {
             ListElement { name: "Bob" }
             ListElement { name: "John" }
             ListElement { name: "Michael" }
         }

         delegate: FocusScope {
             width: childrenRect.width; height: childrenRect.height
             TextInput {
                 focus: true
                 text: name
                 Keys.onReturnPressed: console.log(name)
             }
         }
     }
 }
//![0]
