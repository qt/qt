import Qt.test 1.0

MyQmlObject{
    id: obj
    objectName: "obj"
    function createOne()
    {
        obj.objectProperty = createQmlObject('import Qt.test 1.0; MyQmlObject{objectName:"objectOne"}', obj);
    }

    function createTwo()
    {
        var component = createComponent('dynamicCreation.helper.qml');
        obj.objectProperty = component.createObject();
    }

    function createThree()
    {
        obj.objectProperty = createQmlObject('TypeForDynamicCreation{}', obj);
    }

    function dontCrash()
    {
        var component = createComponent('file-doesnt-exist.qml');
        obj.objectProperty = component.createObject();
    }
}
