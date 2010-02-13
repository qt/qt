import Qt 4.6

XmlListModel {
    source: "model.xml"
    query: "/Pets/Pet"
    XmlRole { name: "name"; query: "/name/string()" }   //starts with '/'
    XmlRole { name: "type"; query: "type" }             //no type
    XmlRole { name: "age"; query: "age/" }              //ends with '/'
    XmlRole { name: "size"; query: "size/number()" }    //wrong type
}
