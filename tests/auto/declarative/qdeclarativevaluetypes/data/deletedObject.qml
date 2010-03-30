import Test 1.0
import Qt 4.6

MyTypeObject {
    property var object

    Script { source: "deletedObject.js" }

    object: MyTypeObject {}
    Component.onCompleted: startup()
    onRunScript: afterDelete()
}
