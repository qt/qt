import Qt 4.7

QtObject {
    property bool dataOK: false

    property string responseText
    property string responseXmlRootNodeValue

    Component.onCompleted: {
        var x = new XMLHttpRequest;

        x.open("GET", "utf16.xml");

        // Test to the end
        x.onreadystatechange = function() {
            if (x.readyState == XMLHttpRequest.DONE) {

                responseText = x.responseText
                if (x.responseXML)
                    responseXmlRootNodeValue = x.responseXML.documentElement.childNodes[0].nodeValue

                dataOK = true;
            }
        }
        x.send()
    }
}

