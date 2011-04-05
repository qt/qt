import QtQuick 1.0
import Qt.test 1.0

Item {
    id: arbitraryVariantConsumer
    property MyArbitraryVariantProvider a: MyArbitraryVariantProvider { id: arbitraryVariantProvider }
    property variant someVariant;
    property int sideEffect: 10

    function callCppFunction() {
        // in this case, we call a nonconst CPP function with variant return value and variant + integer arguments.
        arbitraryVariantConsumer.sideEffect = arbitraryVariantConsumer.sideEffect + 3;
        var tempVariant = arbitraryVariantProvider.setVariantToFilledPixmap(sideEffect + 183, sideEffect + 134, sideEffect + 38, sideEffect + 77, sideEffect + 21);
        someVariant = arbitraryVariantProvider.setVariantAddCount(sideEffect, tempVariant);
    }
}
