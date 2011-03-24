import QtQuick 1.0

import "sssi.js" as SssiJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: SssiJs.testFunc(20)
}
