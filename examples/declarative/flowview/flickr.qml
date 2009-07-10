Rect { 
    radius: 5; 
    pen.width: 1; 
    width:400; 
    height: 120; 
    color: background; 

    XmlListModel {
        id: FeedModel
        source: "http://api.flickr.com/services/feeds/photos_public.gne?format=rss2"
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

    ListView {
        clip: true
        orientation: "Horizontal"
        width: parent.width
        height: 86
        y: 17
        model: FeedModel
        delegate: 
            Item { width: 90; height: 86 
                Rect {
                    anchors.centeredIn: parent
                    width: 86; height: 86; 
                    color: "white"; radius: 5
                    Image { source: imagePath; x: 5; y: 5 }
                }
            }
    }
}

