import Qt 4.6

Rectangle {
    width: 500
    height: 50
    color: "lightBlue"
    Rectangle {
        width: MyText.width
        height: MyText.height
        color: "white"
        anchors.centerIn: parent
        Text {
            id: MyText
            width: NumberAnimation { from: 500; to: 0; running: true; repeat: true; duration: 1000 }
            elide: "ElideRight"
            text: "aaaaaaa bbbbbbb ccccccc\x9Caaaaa bbbbb ccccc\x9Caaa bbb ccc\x9Ca b c"
            text: "Brevity is the soul of wit, and tediousness the limbs and outward flourishes.\x9CBrevity is a great charm of eloquence.\x9CBe concise!\x9CSHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
        }
    }
}
