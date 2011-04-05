import QtQuick 1.0

import "moduleBm.js" as ModuleBmJs

Item {
    id: testQtObject
    property int importedScriptFunctionValue: ModuleBmJs.testFunc(20)
}
