import QtQuick 1.0

import "mlsi.js" as MlsiJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: MlsiJs.testFunc(20)
}
