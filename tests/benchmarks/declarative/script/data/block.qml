import Qt 4.7

Rectangle {
    width: 200; height: 200
    CustomObject { id: theObject }
    function doSomethingDirect() {
        theObject.prop1 = 0;

        for (var i = 0; i < 60; ++i)
            theObject.prop1 += theObject.prop2;

        theObject.prop3 = theObject.prop1;
    }

    function doSomethingLocalObj() {
        theObject.prop1 = 0;

        var incrementObj = theObject;
        for (var i = 0; i < 60; ++i)
            incrementObj.prop1 += incrementObj.prop2;

        incrementObj.prop3 = incrementObj.prop1;
    }

    function doSomethingLocal() {
        theObject.prop1 = 0;

        var increment = theObject.prop2;
        for (var i = 0; i < 60; ++i)
            theObject.prop1 += increment;

        theObject.prop3 = theObject.prop1;
    }
}
