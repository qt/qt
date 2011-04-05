import QtQuick 1.0

import Qt.test 1.0 as QtTest     // module API installed into existing uri

QtObject {
    property int firstProperty: 1
    property int secondProperty: 2
    property int readOnlyProperty: QtTest.qobjectTestProperty
    property int writableProperty: QtTest.qobjectTestWritableProperty

    onFirstPropertyChanged: {
        // In this case, we want to attempt to set the module API property.
        // This should fail, as the module API property is read only.
        if (firstProperty != QtTest.qobjectTestProperty) {
            QtTest.qobjectTestProperty = firstProperty; // should silently fail.
        }
    }

    onSecondPropertyChanged: {
        // In this case, we want to attempt to set the module API property.
        // This should succeed, as the module API property is writable.
        if (secondProperty != QtTest.qobjectTestWritableProperty) {
            QtTest.qobjectTestWritableProperty = secondProperty; // should succeed.
        }
    }
}

