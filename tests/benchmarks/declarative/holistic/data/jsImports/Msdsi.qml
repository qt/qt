import QtQuick 1.0

import "msdsi.js" as MsdsiJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: MsdsiJs.testFunc(20)
}
