import Qt 4.7
import "ImageProviderCore"
//![0]
ListView {
    width: 100; height: 100
    anchors.fill: parent

    model: myModel

    delegate: Component {
        Item {
            width: 100
            height: 50
            Text {
                text: "Loading..."
                anchors.centerIn: parent
            }
            Image {
                source: modelData
                sourceSize: "50x25"
            }
        }
    }
}
//![0]
