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
        SetProperties { target: MyGrid; z: 150 }
    }
    transitions: Transition { 
        SequentialAnimation {
            PauseAnimation { duration: 150 }
            SetPropertyAction { properties: "z" }
        }
    }
    model: XmlListModel {
        property string tags : ""
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

    Item { 
        id: Root
        property bool isSelected: GridView.isCurrentItem && MyGrid.isSelected
        transformOrigin: "Center"
        width: MyGrid.imageWidth; height: MyGrid.imageHeight;

        Image { id: Image; source: url; preserveAspect: true; smooth: true; anchors.fill: parent;
                opacity: (status == 0)?1:0; opacity: Behavior { NumberAnimation { properties: "opacity" } } } 
        Loading { anchors.centeredIn: parent; visible: Image.status }

        states: State {
            name: "selected"
            when: Root.isSelected
            SetProperties { target: Root; scale: 3; z: 100 }
        }
        transitions: [
            Transition {
                toState: "selected"
                SequentialAnimation {
                    PauseAnimation { duration: 150 }
                    SetPropertyAction { properties: "z" }
                    NumberAnimation { properties: "scale"; duration: 150; }
                }
            },
            Transition {
                fromState: "selected"
                SequentialAnimation {
                    NumberAnimation { properties: "scale"; duration: 150 }
                    SetPropertyAction { properties: "z" }
                }
            }
        ]
    }
}

