import Qt 4.6
import ImageProviderCore 1.0
//![0]
ListView {
    width: 100
    height: 100
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
            }
        }
    }
}
//![0]
