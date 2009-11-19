import Qt 4.6

QtObject {
    property string url

    property bool dataOK: false

    Component.onCompleted: {
        var x = new XMLHttpRequest;
        x.open("PUT", url);

        // Test to the end
        x.onreadystatechange = function() {
            if (x.readyState == XMLHttpRequest.DONE) {
                dataOK = (x.responseText == "QML Rocks!\n");
            }
        }

        x.send("My Sent Data");
    }
}
