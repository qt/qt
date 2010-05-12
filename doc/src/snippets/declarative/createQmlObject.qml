import Qt 4.7

Rectangle {
    id: targetItem
    property QtObject newObject

    width: 100
    height: 100
    
    function createIt() {
//![0]
newObject = Qt.createQmlObject('import Qt 4.7; Rectangle {color: "red"; width: 20; height: 20}',
    targetItem, "dynamicSnippet1");
//![0]
    }

    Component.onCompleted: createIt()
}
