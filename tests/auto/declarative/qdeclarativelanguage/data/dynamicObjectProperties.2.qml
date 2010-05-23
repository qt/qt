import Qt 4.7 
import Qt 4.7 as Qt47

Qt.QtObject {
    property Qt47.QtObject objectProperty
    property list<Qt47.QtObject> objectPropertyList

    objectProperty: QtObject {}
    objectPropertyList: QtObject {}
}

