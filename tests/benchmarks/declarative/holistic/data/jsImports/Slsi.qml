import QtQuick 1.0

import "slsi.js" as SlsiJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: SlsiJs.testFunc(20)
}
