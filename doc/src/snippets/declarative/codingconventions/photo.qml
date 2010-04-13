import Qt 4.7

//! [0]
Rectangle {
    id: photo                                               // id on the first line makes it easy to find an object

    property bool thumbnail: false                          // property declarations
    property alias image: photoImage.source

    signal clicked                                          // signal declarations

    function doSomething(x) {                               // javascript functions
        return x + photoImage.width
    }

    x: 20; y: 20; width: 200; height: 150                   // object properties
    color: "gray"                                           // try to group related properties together

    Rectangle {                                             // child objects
        id: border
        anchors.centerIn: parent; color: "white"

        Image { id: photoImage; anchors.centerIn: parent }
    }

    states: State {                                         // states
        name: "selected"
        PropertyChanges { target: border; color: "red" }
    }

    transitions: Transition {                               // transitions
        from: ""; to: "selected"
        ColorAnimation { target: border; duration: 200 }
    }
}
//! [0]

