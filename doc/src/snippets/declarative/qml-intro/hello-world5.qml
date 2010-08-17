import Qt 4.7

Rectangle {
    id: myRectangle
    width: 500
    height: 400

    Text {
        text: "<h1>Hello world again</h1>"
        color: "#002288"
        x: 100; y: 100
    }

//! [positioning the image]
    Image {
        source: "images/qt-logo.png"
        x: 100; y: 150
        width: 150; height: 150
    }
//! [positioning the image]

    color: "lightgray"
}
