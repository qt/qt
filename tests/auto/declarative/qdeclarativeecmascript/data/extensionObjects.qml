import Qt.test 1.0
import Qt 4.7

MyExtendedObject
{
    baseProperty: baseExtendedProperty
    baseExtendedProperty: 13

    coreProperty: extendedProperty
    extendedProperty: 9

    property QtObject nested: MyExtendedObject {
        baseProperty: baseExtendedProperty
        baseExtendedProperty: 13

        coreProperty: extendedProperty
        extendedProperty: 9
    }
}
