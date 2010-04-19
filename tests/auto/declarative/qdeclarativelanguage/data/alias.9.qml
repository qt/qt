import Qt 4.6

QtObject {
   property variant other
   other: Alias4 { id: myAliasObject }

   property int value: myAliasObject.obj.myValue
}

