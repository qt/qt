import Qt 4.7
import "libraryScriptAssert.js" as Test

QtObject {
    id: root
    Component.onCompleted: Test.test(root);
}
