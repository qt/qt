import Qt 4.7
import "createComponent_lib.js" as Test

Item {
    property int status: Component.Null

    Component.onCompleted: {
        status = Test.loadComponent()
    }
}
