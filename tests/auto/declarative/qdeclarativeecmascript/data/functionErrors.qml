import Qt 4.7

QtObject {
    function myFunction() {
        a = 10;
    }

    Component.onCompleted: myFunction();
}

