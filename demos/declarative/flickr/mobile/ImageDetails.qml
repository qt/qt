import Qt 4.6
import "../common" as Common

Flipable {
    id: Container

    property var frontContainer: ContainerFront
    property string photoTitle: ""
    property string photoTags: ""
    property int photoWidth
    property int photoHeight
    property string photoType
    property string photoAuthor
    property string photoDate
    property string photoUrl
    property int rating: 2
    property var prevScale: 1.0

    signal closed

    transform: Rotation {
        id: ItemRotation
        origin.x: Container.width / 2;
        axis.y: 1; axis.z: 0
    }

    front: Item {
        id: ContainerFront; anchors.fill: Container

        Rectangle {
            anchors.fill: parent
            color: "black"; opacity: 0.4
        }

        Connection {
            sender: ToolBar; signal: "button1Clicked()"
            script: if (Container.state=='') Container.state='Back'; else Container.state=''
        }

        Column {
            spacing: 10
            anchors {
                left: parent.left; leftMargin: 20
                right: parent.right; rightMargin: 20
                top: parent.top; topMargin: 180
            }
            Text { id: TitleText; font.bold: true; color: "white"; elide: "ElideRight"; text: Container.photoTitle }
            Text { id: Size; color: "white"; elide: "ElideRight"; text: "<b>Size:</b> " + Container.photoWidth + 'x' + Container.photoHeight }
            Text { id: Type; color: "white"; elide: "ElideRight"; text: "<b>Type:</b> " + Container.photoType }
            Text { id: Author; color: "white"; elide: "ElideRight"; text: "<b>Author:</b> " + Container.photoAuthor }
            Text { id: Date; color: "white"; elide: "ElideRight"; text: "<b>Published:</b> " + Container.photoDate }
            Text { id: TagsLabel; color: "white"; elide: "ElideRight"; text: Container.photoTags == "" ? "" : "<b>Tags:</b> " }
            Text { id: Tags; color: "white"; elide: "ElideRight"; elide: "ElideRight"; text: Container.photoTags }
        }
    }

    back: Item {
        anchors.fill: Container

        Rectangle { anchors.fill: parent; color: "black"; opacity: 0.4 }

        Common.Progress { anchors.centerIn: parent; width: 200; height: 18; progress: BigImage.progress; visible: BigImage.status!=1 }
        Flickable {
            id: Flick; anchors.fill: parent; clip: true
            viewportWidth: ImageContainer.width; viewportHeight: ImageContainer.height

            Item {
                id: ImageContainer
                width: Math.max(BigImage.width * BigImage.scale, Flick.width);
                height: Math.max(BigImage.height * BigImage.scale, Flick.height);

                Image {
                    id: BigImage; source: Container.photoUrl; scale: Slider.value
                    // Center image if it is smaller than the flickable area.
                    x: ImageContainer.width > width*scale ? (ImageContainer.width - width*scale) / 2 : 0
                    y: ImageContainer.height > height*scale ? (ImageContainer.height - height*scale) / 2 : 0
                    smooth: !Flick.moving
                    onStatusChanged : {
                        // Default scale shows the entire image.
                        if (status == 1 && width != 0) {
                            Slider.minimum = Math.min(Flick.width / width, Flick.height / height);
                            prevScale = Math.min(Slider.minimum, 1);
                            Slider.value = prevScale;
                        }
                    }
                }
            }
        }

        Text {
            text: "Image Unavailable"
            visible: BigImage.status == 'Error'
            anchors.centerIn: parent; color: "white"; font.bold: true
        }

        Common.Slider {
            id: Slider; visible: { BigImage.status == 1 && maximum > minimum }
            anchors {
                bottom: parent.bottom; bottomMargin: 65
                left: parent.left; leftMargin: 25
                right: parent.right; rightMargin: 25
            }
            onValueChanged: {
                if (BigImage.width * value > Flick.width) {
                    var xoff = (Flick.width/2 + Flick.viewportX) * value / prevScale;
                    Flick.viewportX = xoff - Flick.width/2;
                }
                if (BigImage.height * value > Flick.height) {
                    var yoff = (Flick.height/2 + Flick.viewportY) * value / prevScale;
                    Flick.viewportY = yoff - Flick.height/2;
                }
                prevScale = value;
            }
        }
    }

    states: State {
        name: "Back"
        PropertyChanges { target: ItemRotation; angle: 180 }
    }

    transitions: Transition {
        SequentialAnimation {
            PropertyAction { target: BigImage; property: "smooth"; value: false }
            NumberAnimation { easing: "easeInOutQuad"; properties: "angle"; duration: 500 }
            PropertyAction { target: BigImage; property: "smooth"; value: !Flick.moving }
        }
    }
}
