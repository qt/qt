import Qt 4.7

QtObject {
    property CustomObject myObject
    myObject: CustomObject { }

    Component.onCompleted: console.log(myObject.greeting)
}
