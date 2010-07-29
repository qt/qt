import Qt 4.7

Rectangle {
    width: 800; height: 450
    
    ListModel {
        id: rssModel
        ListElement { lColor: "red" }
        ListElement { lColor: "green" }
        ListElement { lColor: "yellow" }
        ListElement { lColor: "blue" }
        ListElement { lColor: "purple" }
        ListElement { lColor: "gray" }
        ListElement { lColor: "brown" }
        ListElement { lColor: "thistle" }
    }

    Component {
        id: photoDelegate
        Rectangle {
            id: wrapper
            width: 85; height: 85; color: lColor
            scale: wrapper.PathView.scale

            MouseArea { anchors.fill: parent }
            
            transform: Rotation {
                id: itemRotation; origin.x: wrapper.width/2; origin.y: wrapper.height/2
                axis.y: 1; axis.z: 0; angle: wrapper.PathView.angle
            }
        }
    }

    PathView {
        id: photoPathView; model: rssModel; delegate: photoDelegate
        y: 100; width: 800; height: 330; pathItemCount: 10; z: 1
        path: Path {
            startX: -50; startY: 40;
            
            PathAttribute { name: "scale"; value: 0.5 }
            PathAttribute { name: "angle"; value: -45 }
            
            PathCubic {
                x: 400; y: 220
                control1X: 140; control1Y: 40
                control2X: 210; control2Y: 220
            }
            
            PathAttribute { name: "scale"; value: 1.2  }
            PathAttribute { name: "angle"; value: 0 }
            
            PathCubic {
                x: 850; y: 40
                control2X: 660; control2Y: 40
                control1X: 590; control1Y: 220
            }
            
            PathAttribute { name: "scale"; value: 0.5 }
            PathAttribute { name: "angle"; value: 45 }
        }
    }

    Column {
        Rectangle { width: 20; height: 20; color: "red"; opacity: photoPathView.moving ? 1 : 0 }
        Rectangle { width: 20; height: 20; color: "blue"; opacity: photoPathView.flicking ? 1 : 0 }
    }
}
