import Qt 4.7

XmlListModel {
    property string tags : ""

    source: "http://api.flickr.com/services/feeds/photos_public.gne?"+(tags ? "tags="+tags+"&" : "")
    query: "/feed/entry"
    namespaceDeclarations: "declare default element namespace 'http://www.w3.org/2005/Atom';"

    XmlRole { name: "title"; query: "title/string()" }
    XmlRole { name: "content"; query: "content/string()" }
    XmlRole { name: "hq"; query: "link[@rel='enclosure']/@href/string()" }
}
