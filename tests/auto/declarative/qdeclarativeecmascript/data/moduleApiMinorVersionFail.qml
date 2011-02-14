import QtQuick 1.0

// this qml file attempts to import an invalid version of a qobject module API.

import Qt.test.qobjectApi 1.2 as QtTestMinorVersionQObjectApi // qobject module API installed into existing uri with nonexistent minor version

QtObject {
    property int qobjectMinorVersionTest: QtTestMinorVersionedQObjectApi.qobjectTestProperty
}

