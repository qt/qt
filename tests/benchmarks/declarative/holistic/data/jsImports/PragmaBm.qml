import QtQuick 1.0

import "pragmaBmOne.js" as PragmaBmOneJs
import "pragmaBmTwo.js" as PragmaBmTwoJs

Item {
    id: testQtObject

    // value = 20 + 2 + 9 + (nbr times shared testFunc has been called previously == 0)
    property int importedScriptFunctionValueOne: PragmaBmOneJs.testFuncOne(20)

    // value = 20 + 3 + 9 + (nbr times shared testFunc has been called previously == 1)
    property int importedScriptFunctionValueTwo: PragmaBmTwoJs.testFuncTwo(20)
}
