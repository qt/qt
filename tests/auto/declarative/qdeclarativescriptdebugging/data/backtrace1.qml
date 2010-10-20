import QtQuick 1.0
import Qt.test 1.0
import "backtrace1.js" as Script

Rectangle {
    id: mainRectangle

    property string foo: "Default";
    width: 200
    height: 200


    MyTestObject {

        function append(a, b) {
            return a + " " + b;
        }


        id: testObject;
        someProperty: append("Hello", mainRectangle.foo)

        onSignaled: {
            Script.functionInScript(value , "b");
        }
    }
}
