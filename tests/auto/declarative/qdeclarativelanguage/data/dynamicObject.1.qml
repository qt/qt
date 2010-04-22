import Test 1.0
import Qt 4.7
MyCustomParserType {
    propa: a + 10
    propb: Math.min(a, 10)
    propc: MyPropertyValueSource {}
    onPropA: a
}
