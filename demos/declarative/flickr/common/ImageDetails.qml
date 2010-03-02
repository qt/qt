import Qt 4.6

Flipable {
    id: container

    property var frontContainer: containerFront
    property string photoTitle: ""
    property string photoDescription: ""
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
        id: detailsRotation
        origin.y: container.height / 2;
        origin.x: container.width / 2;
        axis.y: 1; axis.z: 0
    }

    front: Item {
        id: containerFront; anchors.fill: container

        Rectangle {
            anchors.fill: parent
            color: "black"; opacity: 0.4
            border.color: "white"; border.width: 2
        }

        MediaButton {
            id: backButton; x: 630; y: 370; text: "Back"
            onClicked: { container.closed() }
        }

        MediaButton {
            id: moreButton; x: 530; y: 370; text: "View..."
            onClicked: { container.state='Back' }
        }

        Text { id: titleText; style: Text.Raised; styleColor: "black"; color: "white"; elide: Text.ElideRight
               x: 220; y: 30; width: parent.width - 240; text: container.photoTitle; font.pointSize: 22 }

        LikeOMeter { x: 40; y: 250; rating: container.rating }

        Flickable { id: flickable; x: 220; width: 480; height: 210; y: 130; clip: true
                    contentWidth: 480; contentHeight: descriptionText.height

            WebView { id: descriptionText; width: parent.width
                      html: "<style TYPE=\"text/css\">body {color: white;} a:link {color: cyan; text-decoration: underline; }</style>" + container.photoDescription }
        }

        Text { id: size; color: "white"; width: 300; x: 40; y: 300
               text: "<b>Size:</b> " + container.photoWidth + 'x' + container.photoHeight }
        Text { id: type; color: "white"; width: 300; x: 40; anchors.top: size.bottom
               text: "<b>Type:</b> " + container.photoType }

        Text { id: author; color: "white"; width: 300; x: 220; y: 80
               text: "<b>Author:</b> " + container.photoAuthor }
        Text { id: date; color: "white"; width: 300; x: 220; anchors.top: author.bottom
               text: "<b>Published:</b> " + container.photoDate }
        Text { id: tagsLabel; color: "white"; x: 220; anchors.top: date.bottom;
               text: container.photoTags == "" ? "" : "<b>Tags:</b> " }
        Text { id: tags; color: "white"; width: parent.width-x-20;
                anchors.left: tagsLabel.right; anchors.top: date.bottom;
                elide: Text.ElideRight; text: container.photoTags }

        ScrollBar { id: scrollBar; x: 720; y: flickable.y; width: 7; height: flickable.height; opacity: 0;
                    flickableArea: flickable; clip: true }
    }

    back: Item {
        anchors.fill: container

        Rectangle { anchors.fill: parent; color: "black"; opacity: 0.4; border.color: "white"; border.width: 2 }

        Progress { anchors.centerIn: parent; width: 200; height: 18; progress: bigImage.progress; visible: bigImage.status!=1 }
        Flickable {
            id: flick; width: container.width - 10; height: container.height - 10
            x: 5; y: 5; clip: true;
            contentWidth: imageContainer.width; contentHeight: imageContainer.height

            Item {
                id: imageContainer
                width: Math.max(bigImage.width * bigImage.scale, flick.width);
                height: Math.max(bigImage.height * bigImage.scale, flick.height);

                Image {
                    id: bigImage; source: container.photoUrl; scale: slider.value
                    // Center image if it is smaller than the flickable area.
                    x: imageContainer.width > width*scale ? (imageContainer.width - width*scale) / 2 : 0
                    y: imageContainer.height > height*scale ? (imageContainer.height - height*scale) / 2 : 0
                    smooth: !flick.moving
                    onStatusChanged : {
                        // Default scale shows the entire image.
                        if (status == 1 && width != 0) {
                            slider.minimum = Math.min(flick.width / width, flick.height / height);
                            prevScale = Math.min(slider.minimum, 1);
                            slider.value = prevScale;
                        }
                    }
                }
            }
        }

        MediaButton {
            id: backButton2; x: 630; y: 370; text: "Back"; onClicked: { container.state = '' }
        }
        Text {
            text: "Image Unavailable"
            visible: bigImage.status == 'Error'
            anchors.centerIn: parent; color: "white"; font.bold: true
        }

        Slider {
            id: slider; x: 25; y: 374; visible: { bigImage.status == 1 && maximum > minimum }
            onValueChanged: {
                if (bigImage.width * value > flick.width) {
                    var xoff = (flick.width/2 + flick.contentX) * value / prevScale;
                    flick.contentX = xoff - flick.width/2;
                }
                if (bigImage.height * value > flick.height) {
                    var yoff = (flick.height/2 + flick.contentY) * value / prevScale;
                    flick.contentY = yoff - flick.height/2;
                }
                prevScale = value;
            }
        }
    }

    states: [
        State {
            name: "Back"
            PropertyChanges { target: detailsRotation; angle: 180 }
        }
    ]

    transitions: [
        Transition {
            SequentialAnimation {
                PropertyAction {
                    target: bigImage
                    property: "smooth"
                    value: false
                }
                NumberAnimation { easing.type: "InOutQuad"; properties: "angle"; duration: 500 }
                PropertyAction {
                    target: bigImage
                    property: "smooth"
                    value: !flick.moving
                }
            }
        }
    ]
}
