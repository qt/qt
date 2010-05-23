import Qt 4.7
import "include_pragma_outer.js" as Script

Item {
    property int test1

    Component.onCompleted: {
        test1 = Script.callFunction()
    }
}

