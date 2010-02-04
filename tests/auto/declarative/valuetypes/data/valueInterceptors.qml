import Test 1.0

MyTypeObject {
    property int value: 13;

    rect.x: MyOffsetValueInterceptor {}
    rect.x: value
}
