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
        font.pointSize: 24; font.bold: true
        y: 30; anchors.horizontalCenter: page.horizontalCenter
    }
//![2]
}
//![0]
