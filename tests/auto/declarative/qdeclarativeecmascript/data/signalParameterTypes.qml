import Qt.test 1.0

MyQmlObject
{
    id: root
    property int intProperty
    property real realProperty
    property color colorProperty
    property variant variantProperty

    signal mySignal(int a, real b, color c, variant d)

    onMySignal: { intProperty = a; realProperty = b; colorProperty = c; variantProperty = d; }

    onBasicSignal: root.mySignal(10, 19.2, Qt.rgba(1, 1, 0, 1), Qt.rgba(1, 0, 1, 1))
}
