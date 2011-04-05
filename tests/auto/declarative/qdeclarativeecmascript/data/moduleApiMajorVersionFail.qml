import QtQuick 1.0

// this qml file attempts to import an invalid version of a qobject module API.

import Qt.test.qobjectApi 4.0 as QtTestMajorVersionQObjectApi // qobject module API installed into existing uri with nonexistent major version

QtObject {
    property int qobjectMajorVersionTest: QtTestMajorVersionQObjectApi.qobjectTestProperty
}

