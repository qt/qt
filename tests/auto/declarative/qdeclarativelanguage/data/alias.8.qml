import Qt 4.7

QtObject {
   property variant other
   other: Alias3 { id: myAliasObject }

   property int value: myAliasObject.obj.myValue
}

