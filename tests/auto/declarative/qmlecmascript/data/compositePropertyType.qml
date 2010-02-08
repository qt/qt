import Qt 4.6

QtObject {
    property CustomObject myObject
    myObject: CustomObject { }

    Component.onCompleted: console.log(myObject.greeting)
}
