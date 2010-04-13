import Qt 4.6

QtObject {
   property variant other
   other: Alias3 { id: myAliasObject }

   property int value: myAliasObject.obj.myValue
}

