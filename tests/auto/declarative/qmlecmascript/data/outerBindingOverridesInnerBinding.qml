import Qt.test 1.0

MyQmlObject {
    property alias c1: MyConstants.c1
    property alias c2: MyConstants.c2
    property int c3: 0

    objectProperty: ConstantsOverrideBindings { 
        id: MyConstants 
        c2: c3
    }

}

