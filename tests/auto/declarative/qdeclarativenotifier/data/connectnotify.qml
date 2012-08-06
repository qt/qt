import QtQuick 1.0
import Test 1.0

Base {
    id: root
    ExportedClass {
        id: exportedClass
        objectName: "exportedClass"
        onBoundSignal: {}
        property int buw: selfProp
    }

    property int compiledBinding: exportedClass.compiledBindingProp

    normalBinding: {
        Math.abs(12); // Prevent optimization to a compiled binding
        return exportedClass.normalBindingProp
    }

    property int foo: exportedClass.qmlObjectProp
    property int baz: _exportedObject.cppObjectProp

    // Compiled bindings that share a subscription.
    property int compiledBindingShared_1: exportedClass.compiledBindingPropShared
    property int compiledBindingShared_2: exportedClass.compiledBindingPropShared

    function removeCompiledBinding() {
        //console.log("Going to remove compiled binding...")
        root.compiledBinding = 1;
        //console.log("Binding removed!")
    }

    function removeNormalBinding() {
        //console.log("Going to remove normal binding...")
        root.normalBinding = 1;
        //console.log("Binding removed!")
    }

    function removeCompiledBindingShared_1() {
        //console.log("Going to remove compiled binding shared 1...")
        root.compiledBindingShared_1 = 1;
        //console.log("Binding removed!")
    }

    function removeCompiledBindingShared_2() {
        //console.log("Going to remove compiled binding shared 2...")
        root.compiledBindingShared_2 = 1;
        //console.log("Binding removed!")
    }

    function readProperty() {
        var test = exportedClass.unboundProp
    }


    function changeState() {
        //console.log("Changing state...")
        if (root.state == "") root.state = "state1"
        else                  root.state = ""
        //console.log("State changed.")
    }

    property int someValue: 42

    states: State {
        name: "state1"
        PropertyChanges { target: root; someValue: exportedClass.unboundProp }
    }
}
