 import Qt 4.6

 Component {
     id: photoDelegate
     Item {
         id: wrapper; width: 79; height: 79

         Script {
             function photoClicked() {
                 imageDetails.photoTitle = title;
                 imageDetails.photoTags = tags;
                 imageDetails.photoWidth = photoWidth;
                 imageDetails.photoHeight = photoHeight;
                 imageDetails.photoType = photoType;
                 imageDetails.photoAuthor = photoAuthor;
                 imageDetails.photoDate = photoDate;
                 imageDetails.photoUrl = url;
                 imageDetails.rating = 0;
                 scaleMe.state = "Details";
             }
         }

         Item {
             anchors.centerIn: parent
             scale: 0.0
             scale: Behavior { NumberAnimation { easing: "easeInOutQuad"} }
             id: scaleMe

             Rectangle { height: 79; width: 79; id: blackRect;  anchors.centerIn: parent; color: "black"; smooth: true }
             Rectangle {
                 id: whiteRect; width: 77; height: 77; anchors.centerIn: parent; color: "#dddddd"; smooth: true
                 Image { id: thumb; source: imagePath; x: 1; y: 1; smooth: true}
                 Image { source: "images/gloss.png" }
             }

             Connection {
                 sender: toolBar; signal: "button2Clicked()"
                 script: if (scaleMe.state == 'Details' ) scaleMe.state = 'Show';
             }

             states: [
                 State {
                     name: "Show"; when: thumb.status == 1
                     PropertyChanges { target: scaleMe; scale: 1 }
                 },
                 State {
                     name: "Details"
                     PropertyChanges { target: scaleMe; scale: 1 }
                     ParentChange { target: wrapper; parent: imageDetails.frontContainer }
                     PropertyChanges { target: wrapper; x: 20; y: 60; z: 1000 }
                     PropertyChanges { target: background; state: "DetailedView" }
                 }
             ]
             transitions: [
                 Transition {
                     from: "Show"; to: "Details"
                     ParentAction { }
                     NumberAnimation { properties: "x,y"; duration: 500; easing: "easeInOutQuad" }
                 },
                 Transition {
                     from: "Details"; to: "Show"
                     SequentialAnimation {
                         ParentAction { }
                         NumberAnimation { properties: "x,y"; duration: 500; easing: "easeInOutQuad" }
                         PropertyAction { targets: wrapper; properties: "z" }
                     }
                 }
             ]
         }
         MouseArea { anchors.fill: wrapper; onClicked: { photoClicked() } }
     }
 }
