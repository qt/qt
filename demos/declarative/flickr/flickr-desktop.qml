import Qt 4.6

import "common"

Item {
    id: MainWindow; width: 800; height: 450

    property bool showPathView : false

    resources: [
        Component {
        id: PhotoDelegate
        Item {
            id: Wrapper; width: 85; height: 85
            scale: Wrapper.PathView.scale; z: Wrapper.PathView.z

            transform: Rotation {
                id: Rotation; origin.x: Wrapper.width/2; origin.y: Wrapper.height/2
                axis.y: 1; axis.z: 0; angle: Wrapper.PathView.angle
            }

            Connection {
                sender: ImageDetails; signal: "closed()"
                script: {
                    if (Wrapper.state == 'Details') {
                        Wrapper.state = '';
                        ImageDetails.photoUrl = "";
                    }
                }
            }

            Script {
               function photoClicked() {
                   ImageDetails.photoTitle = title;
                   ImageDetails.photoDescription = description;
                   ImageDetails.photoTags = tags;
                   ImageDetails.photoWidth = photoWidth;
                   ImageDetails.photoHeight = photoHeight;
                   ImageDetails.photoType = photoType;
                   ImageDetails.photoAuthor = photoAuthor;
                   ImageDetails.photoDate = photoDate;
                   ImageDetails.photoUrl = url;
                   ImageDetails.rating = 0;
                   Wrapper.state = "Details";
               }
            }

            Rectangle {
                id: WhiteRect; anchors.fill: parent; color: "white"; radius: 5

                Loading { x: 26; y: 26; visible: Thumb.status!=1 }
                Image { id: Thumb; source: imagePath; x: 5; y: 5 }

                Item {
                    id: Shadows
                    Image { source: "common/pics/shadow-right.png"; x: WhiteRect.width; height: WhiteRect.height }
                    Image { source: "common/pics/shadow-bottom.png"; y: WhiteRect.height; width: WhiteRect.width }
                    Image { id: Corner; source: "common/pics/shadow-corner.png"; x: WhiteRect.width; y: WhiteRect.height }
                }
            }

            MouseRegion { anchors.fill: Wrapper; onClicked: { photoClicked() } }

            states: [
                State {
                    name: "Details"
                    PropertyChanges { target: ImageDetails; z: 2 }
                    ParentChange { target: Wrapper; parent: ImageDetails.frontContainer }
                    PropertyChanges { target: Wrapper; x: 45; y: 35; scale: 1; z: 1000 }
                    PropertyChanges { target: Rotation; angle: 0 }
                    PropertyChanges { target: Shadows; opacity: 0 }
                    PropertyChanges { target: ImageDetails; y: 20 }
                    PropertyChanges { target: PhotoGridView; y: "-480" }
                    PropertyChanges { target: PhotoPathView; y: "-480" }
                    PropertyChanges { target: ViewModeButton; opacity: 0 }
                    PropertyChanges { target: TagsEdit; opacity: 0 }
                    PropertyChanges { target: FetchButton; opacity: 0 }
                    PropertyChanges { target: CategoryText; y: "-50" }
                }
            ]

            transitions: [
                Transition {
                    from: "*"; to: "Details"
                    ParentAction { }
                    NumberAnimation { properties: "x,y,scale,opacity,angle"; duration: 500; easing: "easeInOutQuad" }
                },
                Transition {
                    from: "Details"; to: "*"
                    SequentialAnimation {
                        ParentAction { }
                        NumberAnimation { properties: "x,y,scale,opacity,angle"; duration: 500; easing: "easeInOutQuad" }
                        PropertyAction { target: Wrapper; properties: "z" }
                    }
                }
            ]

        }
        }
    ]

    Item {
        id: Background

        anchors.fill: parent

        Image { source: "common/pics/background.png"; anchors.fill: parent }
        RssModel { id: RssModel; tags : TagsEdit.text }
        Loading { anchors.centerIn: parent; visible: RssModel.status }

        GridView {
            id: PhotoGridView; model: RssModel; delegate: PhotoDelegate; cacheBuffer: 100
            cellWidth: 105; cellHeight: 105; x:32; y: 80; width: 800; height: 330; z: 1
        }

        PathView {
            id: PhotoPathView; model: RssModel; delegate: PhotoDelegate
            y: -380; width: 800; height: 330; pathItemCount: 10; z: 1
            path: Path {
                startX: -50; startY: 40;

                PathAttribute { name: "scale"; value: 1 }
                PathAttribute { name: "angle"; value: -45 }

                PathCubic {
                    x: 400; y: 220
                    control1X: 140; control1Y: 40
                    control2X: 210; control2Y: 220
                }

                PathAttribute { name: "scale"; value: 1.2  }
                PathAttribute { name: "z"; value: 1 }
                PathAttribute { name: "angle"; value: 0 }

                PathCubic {
                    x: 850; y: 40
                    control2X: 660; control2Y: 40
                    control1X: 590; control1Y: 220
                }

                PathAttribute { name: "scale"; value: 1 }
                PathAttribute { name: "angle"; value: 45 }
            }

        }

        ImageDetails { id: ImageDetails; width: 750; x: 25; y: 500; height: 410 }

        MediaButton {
            id: ViewModeButton; x: 680; y: 410; text: "View Mode"
            onClicked: { if (MainWindow.showPathView == true) MainWindow.showPathView = false; else MainWindow.showPathView = true }
        }

        MediaButton {
            id: FetchButton
            text: "Update"
            anchors.right: ViewModeButton.left; anchors.rightMargin: 5
            anchors.top: ViewModeButton.top
            onClicked: { RssModel.reload(); }
        }

        MediaLineEdit {
            id: TagsEdit;
            label: "Tags"
            anchors.right: FetchButton.left; anchors.rightMargin: 5
            anchors.top: ViewModeButton.top
        }

        states: [
            State {
                name: "PathView"
                when: MainWindow.showPathView == true
                PropertyChanges { target: PhotoPathView; y: 80 }
                PropertyChanges { target: PhotoGridView; y: -380 }
            }
        ]

        transitions: [
            Transition {
                from: "*"; to: "*"
                NumberAnimation { properties: "y"; duration: 1000; easing: "easeOutBounce(amplitude:0.5)" }
            }
        ]
    }

    Text {
        id: CategoryText;  anchors.horizontalCenter: parent.horizontalCenter; y: 15;
        text: "Flickr - " +
            (RssModel.tags=="" ? "Uploads from everyone" : "Recent Uploads tagged " + RssModel.tags)
        font.pointSize: 20; font.bold: true; color: "white"; style: "Raised"; styleColor: "black"
    }
}
