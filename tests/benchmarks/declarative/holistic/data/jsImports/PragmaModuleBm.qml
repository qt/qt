import QtQuick 1.0

import "pragmaModuleBm.js" as PragmaModuleBmJs

Item {
    id: testQtObject

    // value = 20 + (Qt.test.Enum3 == 2) + 9 + (nbr times shared testFunc has been called previously = 0) + 9 + (nbr times shared testFunc has been called previously = 1)
    property int importedScriptFunctionValue: PragmaModuleBmJs.testFuncThree(20)
}
