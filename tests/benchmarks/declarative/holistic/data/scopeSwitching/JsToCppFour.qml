import QtQuick 1.0
import Qt.test 1.0

Item {
    id: arbitraryVariantConsumer
    property MyArbitraryVariantProvider a: MyArbitraryVariantProvider { id: arbitraryVariantProvider }
    property int sideEffect: 10

    function callCppFunction() {
        // in this case, we call a nonconst CPP function with no return value and an integer argument.
        arbitraryVariantConsumer.sideEffect = arbitraryVariantConsumer.sideEffect + 2;
        arbitraryVariantProvider.setVariantChangeCount(arbitraryVariantConsumer.sideEffect);
    }
}
