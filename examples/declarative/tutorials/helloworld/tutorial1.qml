//![0]
//![3]
import Qt 4.7
//![3]

//![1]
Rectangle {
    id: page
    width: 500; height: 200
    color: "lightgray"
//![1]

//![2]
    Text {
        id: helloText
        text: "Hello world!"
        y: 30
        anchors.horizontalCenter: page.horizontalCenter
        font.pointSize: 24; font.bold: true
    }
//![2]
}
//![0]
