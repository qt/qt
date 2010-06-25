import Qt 4.7
import "qtbug_11600.js" as Test

QtObject {
    id: goo

    property bool test: undefined == goo.Test.foo
}
