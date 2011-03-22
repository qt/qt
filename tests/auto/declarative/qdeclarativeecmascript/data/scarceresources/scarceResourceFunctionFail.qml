import QtQuick 1.0
import Qt.test 1.0

// In this example, a common syntax error will only be "caught"
// when the function is called via:
// QDeclarativeVMEMetaObject::metaCall->invokeMetaMethod()
// We would like to ensure that a useful error message is printed,
// rather than having QScriptValue::call() function fail silently.

QtObject {
    id: root
    property MyScarceResourceObject a: MyScarceResourceObject { id: scarceResourceProvider }
    property variant scarceResourceCopy;

    function retrieveScarceResource() {
        root.scarceResourceCopy = scarceResourceProvider.scarceResource(); // common syntax error, should throw exception
    }

    function releaseScarceResource() {
        root.scarceResourceCopy = null;
    }
}

