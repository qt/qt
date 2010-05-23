import Qt 4.7

VisualDataModel {
    function setRoot() {
        rootIndex = modelIndex(0);
    }
    function setRootToParent() {
        rootIndex = parentModelIndex();
    }
    model: myModel
}
