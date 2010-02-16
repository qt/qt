var myComponent = null;

function createNewBlock() {
    if (myComponent == null)
        myComponent = createComponent("ImageBatch.qml");

    var obj = myComponent.createObject();
    obj.parent = layout;
    obj.offset = maximumColumn + 1;
    obj.ref = imagePanel;
    maximumColumn += 5;
}
