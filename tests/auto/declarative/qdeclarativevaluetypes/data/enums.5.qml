import Test 1.0
import Qt 4.7 as MyQt

MyTypeObject {
    MyQt.Component.onCompleted: {
        font.capitalization = MyQt.Font.AllUppercase
    }
}


