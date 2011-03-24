import QtQuick 1.0

import "mldsi.js" as MldsiJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: MldsiJs.testFunc(20)
}
