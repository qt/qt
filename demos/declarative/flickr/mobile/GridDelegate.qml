 import Qt 4.7

 Component {
     id: photoDelegate
     Item {
         id: wrapper; width: 79; height: 79

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

         Item {
             anchors.centerIn: parent
             scale: 0.0
             Behavior on scale { NumberAnimation { easing.type: "InOutQuad"} }
             id: scaleMe

             Rectangle { height: 79; width: 79; id: blackRect;  anchors.centerIn: parent; color: "black"; smooth: true }
             Rectangle {
                 id: whiteRect; width: 77; height: 77; anchors.centerIn: parent; color: "#dddddd"; smooth: true
                 Image { id: thumb; source: imagePath; x: 1; y: 1; smooth: true}
                 Image { source: "images/gloss.png" }
             }

             Connections {
                 target: toolBar
                 onButton2Clicked: if (scaleMe.state == 'Details' ) scaleMe.state = 'Show'
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
                     ParentAnimation {
                         NumberAnimation { properties: "x,y"; duration: 500; easing.type: "InOutQuad" }
                     }
                 },
                 Transition {
                     from: "Details"; to: "Show"
                     SequentialAnimation {
                         ParentAnimation {
                            NumberAnimation { properties: "x,y"; duration: 500; easing.type: "InOutQuad" }
                         }
                         PropertyAction { targets: wrapper; properties: "z" }
                     }
                 }
             ]
         }
         MouseArea { anchors.fill: wrapper; onClicked: { photoClicked() } }
     }
 }
