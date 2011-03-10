import QtQuick 1.0
import Qt.test 1.0
import "scarceResourceTest.js" as ScarceResourceProviderJs

// In this case, multiple scarce resource are explicity preserved
// and then explicitly destroyed, while others are automatically
// managed.  Since none are manually preserved without subsequently
// being destroyed, after the evaluation of the binding the
// scarce resource should be detached.

QtObject {
    property MyScarceResourceObject a;
    a: MyScarceResourceObject { id: scarceResourceProvider }
    property int scarceResourceTest: ScarceResourceProviderJs.importPreservedScarceResourceFromMultiple(scarceResourceProvider), 100
}

