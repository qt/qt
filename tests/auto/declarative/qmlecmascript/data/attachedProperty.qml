import Qt.test 1.0
import Qt.test 1.0 as Namespace

MyQmlObject {
    id: Me
    property int a: MyQmlObject.value
    property int b: Namespace.MyQmlObject.value
    property int c: Me.Namespace.MyQmlObject.value
    property int d: Me.Namespace.MyQmlObject.value
}

