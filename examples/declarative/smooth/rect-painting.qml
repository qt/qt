import Qt 4.6

Rect {
    width: 900; height: 500
    color: "white"

    Rect {
        anchors.top: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#eeeeee"
    }

    GridPositioner {
        anchors.centerIn: parent
        columns: 8; rows:4; spacing: 30

        MyRect { color: "lightsteelblue" }
        MyRect { color: "lightsteelblue"; border: "gray" }
        MyRect { color: "lightsteelblue"; radius: 10 }
        MyRect { color: "lightsteelblue"; radius: 10; border: "gray" }
        GradientRect { color: "lightsteelblue" }
        GradientRect { color: "lightsteelblue"; border: "gray" }
        GradientRect { color: "lightsteelblue"; radius: 10 }
        GradientRect { color: "lightsteelblue"; radius: 10; border: "gray" }

        MyRect { color: "thistle"; rotation: 10 }
        MyRect { color: "thistle"; border: "gray"; rotation: 10 }
        MyRect { color: "thistle"; radius: 10; rotation: 10 }
        MyRect { color: "thistle"; radius: 10; border: "gray"; rotation: 10 }
        GradientRect { color: "thistle"; rotation: 10 }
        GradientRect { color: "thistle"; border: "gray"; rotation: 10 }
        GradientRect { color: "thistle"; radius: 10; rotation: 10 }
        GradientRect { color: "thistle"; radius: 10; border: "gray"; rotation: 10 }

        MyRect { color: "lightsteelblue"; smooth: true }
        MyRect { color: "lightsteelblue"; border: "gray"; smooth: true }
        MyRect { color: "lightsteelblue"; radius: 10; smooth: true }
        MyRect { color: "lightsteelblue"; radius: 10; border: "gray"; smooth: true }
        GradientRect { color: "lightsteelblue"; smooth: true }
        GradientRect { color: "lightsteelblue"; border: "gray"; smooth: true }
        GradientRect { color: "lightsteelblue"; radius: 10; smooth: true }
        GradientRect { color: "lightsteelblue"; radius: 10; border: "gray"; smooth: true }

        MyRect { color: "thistle"; rotation: 10; smooth: true }
        MyRect { color: "thistle"; border: "gray"; rotation: 10; smooth: true }
        MyRect { color: "thistle"; radius: 10; rotation: 10; smooth: true }
        MyRect { color: "thistle"; radius: 10; border: "gray"; rotation: 10; smooth: true }
        GradientRect { color: "thistle"; rotation: 10; smooth: true }
        GradientRect { color: "thistle"; border: "gray"; rotation: 10; smooth: true }
        GradientRect { color: "thistle"; radius: 10; rotation: 10; smooth: true }
        GradientRect { color: "thistle"; radius: 10; border: "gray"; rotation: 10; smooth: true }
    }

    Text {
        text: "smooth: false"; font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top
    }
    Text {
        text: "smooth: true"; font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom
    }
}
