import Qt 4.6

import "common"

Item {
    id: mainWindow; width: 800; height: 450

    property bool showPathView : false

    resources: [
        Component {
        id: photoDelegate
        Item {
            id: wrapper; width: 85; height: 85
            scale: wrapper.PathView.scale ? wrapper.PathView.scale : 1
            z: wrapper.PathView.z ? wrapper.PathView.z : 0

            transform: Rotation {
                id: itemRotation; origin.x: wrapper.width/2; origin.y: wrapper.height/2
                axis.y: 1; axis.z: 0
                angle: wrapper.PathView.angle ? wrapper.PathView.angle : 0
            }

            Connection {
                sender: imageDetails; signal: "closed()"
                script: {
                    if (wrapper.state == 'Details') {
                        wrapper.state = '';
                        imageDetails.photoUrl = "";
                    }
                }
            }

            Script {
               function photoClicked() {
                   imageDetails.photoTitle = title;
                   imageDetails.photoDescription = description;
                   imageDetails.photoTags = tags;
                   imageDetails.photoWidth = photoWidth;
                   imageDetails.photoHeight = photoHeight;
                   imageDetails.photoType = photoType;
                   imageDetails.photoAuthor = photoAuthor;
                   imageDetails.photoDate = photoDate;
                   imageDetails.photoUrl = url;
                   imageDetails.rating = 0;
                   wrapper.state = "Details";
               }
            }

            Rectangle {
                id: whiteRect; anchors.fill: parent; color: "white"; radius: 5

                Loading { x: 26; y: 26; visible: thumb.status!=1 }
                Image { id: thumb; source: imagePath; x: 5; y: 5 }

                Item {
                    id: shadows
                    Image { source: "common/pics/shadow-right.png"; x: whiteRect.width; height: whiteRect.height }
                    Image { source: "common/pics/shadow-bottom.png"; y: whiteRect.height; width: whiteRect.width }
                    Image { id: corner; source: "common/pics/shadow-corner.png"; x: whiteRect.width; y: whiteRect.height }
                }
            }

            MouseRegion { anchors.fill: wrapper; onClicked: { photoClicked() } }

            states: [
                State {
                    name: "Details"
                    PropertyChanges { target: imageDetails; z: 2 }
                    ParentChange { target: wrapper; parent: imageDetails.frontContainer }
                    PropertyChanges { target: wrapper; x: 45; y: 35; scale: 1; z: 1000 }
                    PropertyChanges { target: itemRotation; angle: 0 }
                    PropertyChanges { target: shadows; opacity: 0 }
                    PropertyChanges { target: imageDetails; y: 20 }
                    PropertyChanges { target: photoGridView; y: -480 }
                    PropertyChanges { target: photoPathView; y: -480 }
                    PropertyChanges { target: viewModeButton; opacity: 0 }
                    PropertyChanges { target: tagsEdit; opacity: 0 }
                    PropertyChanges { target: fetchButton; opacity: 0 }
                    PropertyChanges { target: categoryText; y: "-50" }
                }
            ]

            transitions: [
                Transition {
                    from: "*"; to: "Details"
                    SequentialAnimation {
                        ParentAction { }
                        NumberAnimation { properties: "x,y,scale,opacity,angle"; duration: 500; easing: "easeInOutQuad" }
                    }
                },
                Transition {
                    from: "Details"; to: "*"
                    SequentialAnimation {
                        ParentAction { }
                        NumberAnimation { properties: "x,y,scale,opacity,angle"; duration: 500; easing: "easeInOutQuad" }
                        PropertyAction { targets: wrapper; properties: "z" }
                    }
                }
            ]

        }
        }
    ]

    Item {
        id: background

        anchors.fill: parent

        Image { source: "common/pics/background.png"; anchors.fill: parent }
        RssModel { id: rssModel; tags : tagsEdit.text }
        Loading { anchors.centerIn: parent; visible: rssModel.status == 2 }

        GridView {
            id: photoGridView; model: rssModel; delegate: photoDelegate; cacheBuffer: 100
            cellWidth: 105; cellHeight: 105; x:32; y: 80; width: 800; height: 330; z: 1
        }

        PathView {
            id: photoPathView; model: rssModel; delegate: photoDelegate
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

        ImageDetails { id: imageDetails; width: 750; x: 25; y: 500; height: 410 }

        MediaButton {
            id: viewModeButton; x: 680; y: 410; text: "View Mode"
            onClicked: { if (mainWindow.showPathView == true) mainWindow.showPathView = false; else mainWindow.showPathView = true }
        }

        MediaButton {
            id: fetchButton
            text: "Update"
            anchors.right: viewModeButton.left; anchors.rightMargin: 5
            anchors.top: viewModeButton.top
            onClicked: { rssModel.reload(); }
        }

        MediaLineEdit {
            id: tagsEdit;
            label: "Tags"
            anchors.right: fetchButton.left; anchors.rightMargin: 5
            anchors.top: viewModeButton.top
        }

        states: State {
            name: "PathView"
            when: mainWindow.showPathView == true
            PropertyChanges { target: photoPathView; y: 80 }
            PropertyChanges { target: photoGridView; y: -380 }
        }

        transitions: [
            Transition {
                from: "*"; to: "*"
                NumberAnimation { properties: "y"; duration: 1000; easing: "easeOutBounce(amplitude:0.5)" }
            }
        ]
    }

    Text {
        id: categoryText;  anchors.horizontalCenter: parent.horizontalCenter; y: 15;
        text: "Flickr - " +
            (rssModel.tags=="" ? "Uploads from everyone" : "Recent Uploads tagged " + rssModel.tags)
        font.pointSize: 20; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
    }
}
