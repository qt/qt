import QtQuick 1.0

import "mssi.js" as MssiJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: MssiJs.testFunc(20)
}
