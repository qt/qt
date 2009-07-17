var myComponent = null;

function createNewBlock() {
    if (myComponent == null) 
        myComponent = createComponent("ImageBatch.qml");

    var obj = myComponent.createObject();
    obj.parent = MyLayout;
    obj.offset = maximumColumn + 1;
    obj.ref = ImagePanel;
    maximumColumn += 5;
}
