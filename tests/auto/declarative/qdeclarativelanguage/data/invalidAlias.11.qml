import QtQuick 1.0

QtObject {
    id: main
    property variant v: Text{
        text: "variant text"
    }
    property alias foo: main.v.text
}
