import Qt 4.6

QtObject {
   property var other
   other: Alias { id: MyAliasObject }

   property alias value: MyAliasObject.aliasValue
   property alias value2: MyAliasObject.value
}

