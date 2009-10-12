import Qt 4.6
import Test 1.0

Object {
    property Object myProperty
    property Object myProperty2
    property Object myProperty3
    property Object myProperty4
    property MyQmlObject myProperty5
    property MyQmlObject myProperty6

    myProperty: CompositeType {}
    myProperty2: CompositeType2 {}
    myProperty3: CompositeType3 {}
    myProperty4: CompositeType4 {}
    myProperty5: CompositeType2 {}
    myProperty6: CompositeType4 {}
}
