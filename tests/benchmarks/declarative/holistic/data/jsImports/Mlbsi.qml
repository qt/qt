import QtQuick 1.0

import "mlbsi.js" as MlbsiJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: MlbsiJs.testFunc(20)
}
