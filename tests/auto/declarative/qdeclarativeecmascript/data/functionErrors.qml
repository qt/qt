import Qt 4.6

QtObject {
    function myFunction() {
        a = 10;
    }

    Component.onCompleted: myFunction();
}

