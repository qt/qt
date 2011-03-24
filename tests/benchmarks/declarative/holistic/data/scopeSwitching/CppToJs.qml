import QtQuick 1.0
import "cppToJs.js" as CppToJs

Item {
    id: jsConsumer
    property int sideEffect: 10

    function callJsFunction() {
        jsConsumer.sideEffect = jsConsumer.sideEffect + CppToJs.nextValue;
    }
}
