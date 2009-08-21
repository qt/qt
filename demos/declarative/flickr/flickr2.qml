import Qt 4.6

import "content"

Item {
    id: MainWindow; width: 800; height: 450

    property bool showPathView : false

        VisualModel {
            id: MyVisualModel
            model:
                XmlListModel {
                    id: FeedModel
                    property string tags : TagsEdit.text
                    source: "http://api.flickr.com/services/feeds/photos_public.gne?"+(tags ? "tags="+tags+"&" : "")+"format=rss2"
                    query: "/rss/channel/item"
                    namespaceDeclarations: "declare namespace media=\"http://search.yahoo.com/mrss/\";"

                    XmlRole { name: "title"; query: "title/string()" }
                    XmlRole { name: "imagePath"; query: "media:thumbnail/@url/string()" }
                    XmlRole { name: "url"; query: "media:content/@url/string()" }
                    XmlRole { name: "description"; query: "description/string()" }
                    XmlRole { name: "tags"; query: "media:category/string()" }
                    XmlRole { name: "photoWidth"; query: "media:content/@width/string()" }
                    XmlRole { name: "photoHeight"; query: "media:content/@height/string()" }
                    XmlRole { name: "photoType"; query: "media:content/@type/string()" }
                    XmlRole { name: "photoAuthor"; query: "author/string()" }
                    XmlRole { name: "photoDate"; query: "pubDate/string()" }
                }

            delegate: Package {
                Item {
                    id: Wrapper; width: 85; height: 85; scale: {1.0}
                    z: PathViewPackage.PathView.z
                    property real angle: 0 * 0

                    transform: [
                        Rotation {
                            id: Rotation; origin.x: 30; axis.x: 30; axis.y: 60; axis.z: 0
                            angle: Wrapper.angle
                        }
                    ]

                    Connection {
                        sender: Background.imageDetails; signal: "closed()"
                        script: { if (Wrapper.state == 'Details') Wrapper.state = '' }
                    }

                    Script {
                        function photoClicked() {
                            Background.imageDetails.photoTitle = title;
                            Background.imageDetails.photoDescription = description;
                            Background.imageDetails.photoTags = tags;
                            Background.imageDetails.photoWidth = photoWidth;
                            Background.imageDetails.photoHeight = photoHeight;
                            Background.imageDetails.photoType = photoType;
                            Background.imageDetails.photoAuthor = photoAuthor;
                            Background.imageDetails.photoDate = photoDate;
                            Background.imageDetails.photoUrl = url;
                            Background.imageDetails.rating = 0;
                            Wrapper.state = "Details";
                        }
                    }

                    Rectangle {
                        id: WhiteRect; anchors.fill: parent; color: "white"; radius: 5

                        Loading { x: 26; y: 26; visible: Thumb.status!=1 }
                        Image { id: Thumb; source: imagePath; x: 5; y: 5 }

                        Item {
                            id: Shadows
                            Image { source: "content/pics/shadow-right.png"; x: WhiteRect.width; height: WhiteRect.height }
                            Image { source: "content/pics/shadow-bottom.png"; y: WhiteRect.height; width: WhiteRect.width }
                            Image { id: Corner; source: "content/pics/shadow-corner.png"; x: WhiteRect.width; y: WhiteRect.height }
                        }
                    }

                    MouseRegion { anchors.fill: Wrapper; onClicked: { photoClicked() } }

                    states: [
                        State {
                            name: "Details"
                            PropertyChanges { target: Background.imageDetails; z: 2 }
                            ParentChange { target: Wrapper; parent: Background.imageDetails.frontContainer }
                            PropertyChanges { target: Wrapper; x: 45; y: 35; scale: 1; z: 1000 }
                            PropertyChanges { target: Rotation; angle: 0 }
                            PropertyChanges { target: Shadows; opacity: 0 }
                            PropertyChanges { target: Background.imageDetails; y: 20 }
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
                            fromState: "*"; toState: "Details"
                            ParentAction { }
                            NumberAnimation { properties: "x,y,scale,opacity,angle"; duration: 500; easing: "easeInOutQuad" }
                        },
                        Transition {
                            fromState: "Details"; toState: "*"
                            SequentialAnimation {
                                ParentAction { }
                                NumberAnimation { properties: "x,y,scale,opacity,angle"; duration: 500; easing: "easeInOutQuad" }
                                PropertyAction { target: Wrapper; properties: "z" }
                            }
                        }
                    ]
                }

                Item {
                    Package.name: "pathView"
                    id: PathViewPackage; width: 85; height: 85
                }

                Item {
                    Package.name: "gridView"
                    id: GridViewPackage; width: 85; height: 85
                }

                Item {
                    id: MyItem
                    state: MainWindow.showPathView ? "pathView" : "gridView"
                    states: [
                        State {
                            name: "gridView"
                            PropertyChanges { target: Wrapper; explicit: true; property: "moveToParent"; value: GridViewPackage }
                        },
                        State {
                            name: "pathView"
                            PropertyChanges { target: Wrapper; scale: PathViewPackage.PathView.scale; angle: PathViewPackage.PathView.angle; }
                            PropertyChanges { target: Wrapper; explicit: true; moveToParent: PathViewPackage }
                        }
                    ]
                    transitions: [
                        Transition {
                            toState: "pathView"
                            SequentialAnimation {
                                PropertyAction { target: Wrapper; property: "moveToParent" }
                                ParallelAnimation {
                                    NumberAnimation {
                                        target: Wrapper
                                        properties: "x,y"
                                        to: 0
                                        easing: "easeOutQuad"
                                        duration: 350
                                    }
                                    NumberAnimation { target: Wrapper; properties: "scale,angle"; duration: 350 }
                                }
                            }
                        },
                        Transition {
                            toState: "gridView"
                            SequentialAnimation {
                                PauseAnimation { duration: Math.floor(index/7)*100 }
                                PropertyAction { target: Wrapper; property: "moveToParent" }
                                ParallelAnimation {
                                    NumberAnimation {
                                        target: Wrapper
                                        properties: "x,y"
                                        to: 0
                                        easing: "easeOutQuad"
                                        duration: 250
                                    }
                                    NumberAnimation { target: Wrapper; properties: "scale,angle"; duration: 250 }
                                }
                            }
                        }
                    ]
                }
            }
        }


    Item {
        id: Background
        property var imageDetails: ImageDetails

        Image { source: "content/pics/background.png" }

        GridView {
            id: PhotoGridView; model: MyVisualModel.parts.gridView
            cellWidth: 105; cellHeight: 105; x:32; y: 80; width: 800; height: 330; z: 1
            cacheBuffer: 100
        }

        PathView {
            id: PhotoPathView; model: MyVisualModel.parts.pathView
            y: 80; width: 800; height: 330; z: 1
            pathItemCount: 10; snapPosition: 0.001
            path: Path {
                startX: -150; startY: 40;

                PathAttribute { name: "scale"; value: 0.9 }
                PathAttribute { name: "angle"; value: -45 }
                PathPercent { value: 0 }
                PathLine { x: -50; y: 40 }

                PathPercent { value: 0.001 }

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

                PathPercent { value: 0.999 }
                PathLine { x: 950; y: 40 }
                PathPercent { value: 1.0 }
                PathAttribute { name: "scale"; value: 0.9 }
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
            onClicked: { FeedModel.reload(); }
        }

        MediaLineEdit {
            id: TagsEdit;
            label: "Tags"
            anchors.right: FetchButton.left; anchors.rightMargin: 5
            anchors.top: ViewModeButton.top
        }

    }

    Text {
        id: CategoryText;  anchors.horizontalCenter: parent.horizontalCenter; y: 15;
        text: "Flickr - " +
            (FeedModel.tags=="" ? "Uploads from everyone" : "Recent Uploads tagged " + FeedModel.tags)
        font.pointSize: 16; font.bold: true; color: "white"; style: "Raised"; styleColor: "black"
    }
}
