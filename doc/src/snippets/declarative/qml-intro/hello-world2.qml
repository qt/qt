import Qt 4.7

Rectangle {
    id: myRectangle
    width: 500
    height: 400

//! [updated text]
    Text {
        text: "<h2>Hello World</h2>"; color: "darkgreen"
        x: 100; y:100
    }
//! [updated text]

    color: "lightgray"
}
