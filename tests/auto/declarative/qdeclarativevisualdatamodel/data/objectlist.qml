import Qt 4.7

ListView {
    width: 100
    height: 100
    anchors.fill: parent
    model: myModel
    delegate: Component {
        Rectangle {
            height: 25
            width: 100
            color: model.modelData.color
            Text { objectName: "name"; text: name }
        }
    }
}
