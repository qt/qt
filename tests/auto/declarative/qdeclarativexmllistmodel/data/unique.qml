import Qt 4.7

XmlListModel {
    source: "model.xml"
    query: "/Pets/Pet"
    XmlRole { name: "name"; query: "name/string()" }
    XmlRole { name: "name"; query: "type/string()" }
}
