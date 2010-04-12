import Qt 4.6
import "libraryScriptAssert.js" as Test

QtObject {
    id: root
    Component.onCompleted: Test.test(root);
}
