import Qt 4.7

Rectangle {
    width: 500
    height: 50
    color: "lightBlue"
    Rectangle {
        width: myText.width
        height: myText.height
        color: "white"
        anchors.centerIn: parent
        Text {
            id: myText
            width: NumberAnimation { from: 500; to: 0; loops: Animation.Infinite; duration: 1000 }
            elide: "ElideRight"
            text: "Brevity is the soul of wit, and tediousness the limbs and outward flourishes.\x9CBrevity is a great charm of eloquence.\x9CBe concise!\x9CSHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
        }
    }
}
