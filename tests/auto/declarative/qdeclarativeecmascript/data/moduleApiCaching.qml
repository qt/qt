import QtQuick 1.0

import Qt.test 1.0 as QtTest                                      // module API installed into existing uri
import Qt.test.scriptApi 1.0 as QtTestScriptApi                   // script module API installed into new uri
import Qt.test.qobjectApiParented 1.0 as QtTestParentedQObjectApi // qobject (with parent) module API installed into a new uri

QtObject {
    property int existingUriTest: QtTest.qobjectTestProperty
    property int scriptTest: QtTestScriptApi.scriptTestProperty
    property int qobjectParentedTest: QtTestParentedQObjectApi.qobjectTestProperty
}

