import Test 1.0
import Qt 4.6

MyQmlObject {
    property int a: Math.max(10, 9)
    property int b: 11 
    Component.onDestruction: console.log("Destruction " + a + " " + b);
}
