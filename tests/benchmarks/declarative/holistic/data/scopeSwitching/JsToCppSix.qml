import QtQuick 1.0
import Qt.test 1.0

Item {
    id: arbitraryVariantConsumer
    property MyArbitraryVariantProvider a: MyArbitraryVariantProvider { id: arbitraryVariantProvider }
    property int sideEffect: 10

    function callCppFunction() {
        // in this case, we call a nonconst CPP function with an integer return value and no arguments.
        arbitraryVariantConsumer.sideEffect = arbitraryVariantConsumer.sideEffect + arbitraryVariantProvider.modifyVariantChangeCount();
    }
}
