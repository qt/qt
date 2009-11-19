import Qt.test 1.0
import Qt 4.6

QtObject {
    property MyExtendedObject a;
    a: MyExtendedObject { id: Root }
    property int b: Math.max(Root.extendedProperty, 0)
}
