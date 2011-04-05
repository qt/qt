import QtQuick 1.0

import Qt.test 1.0 as QtTest                                    // module API installed into existing uri
import Qt.test.scriptApi 1.0 as QtTestScriptApi                 // script module API installed into new uri
import Qt.test.qobjectApi 1.0 as QtTestQObjectApi               // qobject module API installed into new uri
import Qt.test.qobjectApi 1.3 as QtTestMinorVersionQObjectApi   // qobject module API installed into existing uri with new minor version
import Qt.test.qobjectApi 2.0 as QtTestMajorVersionQObjectApi   // qobject module API installed into existing uri with new major version
import Qt.test.qobjectApiParented 1.0 as QtTestParentedQObjectApi // qobject (with parent) module API installed into a new uri

QtObject {
    property int existingUriTest: QtTest.qobjectTestProperty
    property int scriptTest: QtTestScriptApi.scriptTestProperty
    property int qobjectTest: QtTestQObjectApi.qobjectTestProperty
    property int qobjectMinorVersionTest: QtTestMinorVersionQObjectApi.qobjectTestProperty
    property int qobjectMajorVersionTest: QtTestMajorVersionQObjectApi.qobjectTestProperty
    property int qobjectParentedTest: QtTestParentedQObjectApi.qobjectTestProperty
}

