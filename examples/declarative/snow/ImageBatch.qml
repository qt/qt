import Qt 4.6

GridView {
    id: MyGrid
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
        name: "selected"; when: MyGrid.isSelected
        PropertyChanges { target: MyGrid; z: 150 }
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

    Item { 
        id: Root
        property bool isSelected: GridView.isCurrentItem && MyGrid.isSelected
        transformOrigin: "Center"
        width: MyGrid.imageWidth; height: MyGrid.imageHeight;

        Image { id: Image; source: url; fillMode: "PreserveAspectFit"; smooth: true; anchors.fill: parent;
                opacity: (status == 1)?1:0; opacity: Behavior { NumberAnimation { properties: "opacity" } } } 
        Loading { anchors.centerIn: parent; visible: Image.status!=1 }

        states: State {
            name: "selected"
            when: Root.isSelected
            PropertyChanges { target: Root; scale: 3; z: 100 }
        }
        transitions: [
            Transition {
                toState: "selected"
                SequentialAnimation {
                    PauseAnimation { duration: 150 }
                    PropertyAction { properties: "z" }
                    NumberAnimation { properties: "scale"; duration: 150; }
                }
            },
            Transition {
                fromState: "selected"
                SequentialAnimation {
                    NumberAnimation { properties: "scale"; duration: 150 }
                    PropertyAction { properties: "z" }
                }
            }
        ]
    }
}

