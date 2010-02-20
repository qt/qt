import Qt 4.6

GridView {
    id: grid
    property int offset: 0
    property var ref
    property bool isSelected: ref.selectedItemColumn >= offset && ref.selectedItemColumn < offset + 5

    currentIndex: (ref.selectedItemColumn  - offset) + ref.selectedItemRow * 5

    property int imageWidth: ref.imageWidth
    property int imageHeight: ref.imageHeight

    width: 5 * imageWidth
    height: 4 * imageHeight
    cellWidth: imageWidth
    cellHeight: imageHeight

    states: State {
        name: "selected"; when: grid.isSelected
        PropertyChanges { target: grid; z: 150 }
    }
    transitions: Transition {
        SequentialAnimation {
            PauseAnimation { duration: 150 }
            PropertyAction { properties: "z" }
        }
    }
    model: XmlListModel {
        property string tags : ""
        source: "http://api.flickr.com/services/feeds/photos_public.gne?"+(tags ? "tags="+tags+"&" : "")+"format=rss2"
        query: "/rss/channel/item"
        namespaceDeclarations: "declare namespace media=\"http://search.yahoo.com/mrss/\";"

        XmlRole { name: "url"; query: "media:content/@url/string()" }
    }

    delegate: Item {
        id: root
        property bool isSelected: GridView.isCurrentItem && grid.isSelected
        transformOrigin: Item.Center
        width: grid.imageWidth; height: grid.imageHeight;

        Image { id: flickrImage; source: url; fillMode: Image.PreserveAspectFit; smooth: true; anchors.fill: parent;
                opacity: (status == Image.Ready)?1:0; opacity: Behavior { NumberAnimation { } } }
        Loading { anchors.centerIn: parent; visible: flickrImage.status!=1 }

        states: State {
            name: "selected"
            when: root.isSelected
            PropertyChanges { target: root; scale: 3; z: 100 }
        }
        transitions: [
            Transition {
                to: "selected"
                SequentialAnimation {
                    PauseAnimation { duration: 150 }
                    PropertyAction { properties: "z" }
                    NumberAnimation { properties: "scale"; duration: 150; }
                }
            },
            Transition {
                from: "selected"
                SequentialAnimation {
                    NumberAnimation { properties: "scale"; duration: 150 }
                    PropertyAction { properties: "z" }
                }
            }
        ]
    }
}

