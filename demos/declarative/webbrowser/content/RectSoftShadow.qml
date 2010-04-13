import Qt 4.7

Item {
    BorderImage {
        source: "pics/softshadow-left.sci"
        x: -16
        y: -16
        width: 16
        height: parent.height+32
    }
    BorderImage {
        source: "pics/softshadow-right.sci"
        x: parent.width
        y: -16
        width: 16
        height: parent.height+32
    }
    Image {
        source: "pics/softshadow-top.png"
        x: 0
        y: -16
        width: parent.width
        height: 16
    }
    Image {
        source: "pics/softshadow-bottom.png"
        x: 0
        y: parent.height
        width: parent.width
        height: 16
    }
}
