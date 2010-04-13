import Qt 4.7
import "content"

Rectangle {
    id: window
    
    width: 480; height: 320
    color: "darkgreen"

    Row {
        anchors.centerIn: parent; spacing: 30
        Card { image: "content/9_club.png"; angle: 180; yAxis: 1 }
        Card { image: "content/5_heart.png"; angle: 540; xAxis: 1 }
    }
}
