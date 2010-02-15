//![0]
import Qt 4.6

Rectangle {
    id: page
    function forceClose() {
        page.closed();
        page.opacity = 0;
    }
    function show(txt) {
        myText.text = txt;
        page.opacity = 1;
    }
    signal closed();
    color: "white"; border.width: 1; width: myText.width + 20; height: 60;
    opacity: 0
    opacity: Behavior { 
        NumberAnimation { duration: 1000 }
    }
    Text { id: myText; anchors.centerIn: parent; text: "Hello World!" }
    MouseRegion { id: mr; anchors.fill: parent; onClicked: forceClose(); }
}
//![0]
