import QtQuick 1.0

import "msbsi.js" as MsbsiJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: MsbsiJs.testFunc(20)
}
