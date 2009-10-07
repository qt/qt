import Qt.test 1.0

MyQmlObject{
    id: obj
    objectName: "obj"
    function create()
    {
        obj.objectProperty = createQmlObject('import Qt.test 1.0; MyQmlObject{objectName:"emptyObject"}', obj);
    }

    function killIt(wait)
    {
        if(obj.objectProperty == undefined || obj.objectProperty == null)
            return;
        if(wait > 0)
            obj.objectProperty.destroy(wait);
        else
            obj.objectProperty.destroy();
    }
}
