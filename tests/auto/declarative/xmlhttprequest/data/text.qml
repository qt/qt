import Qt 4.6

Object {
    property bool xmlTest: false
    property bool dataOK: false

    Script {
        function checkText(text, whitespacetext)
        {
            if (text.wholeText != "Hello world!")
                return;

            if (text.isElementContentWhitespace != false)
                return;

            if (whitespacetext.wholeText != "   ")
                return;

            if (whitespacetext.isElementContentWhitespace != true)
                return;

            xmlTest = true;
        }

        function checkXML(document)
        {
            checkText(document.documentElement.childNodes[0].childNodes[0],
                      document.documentElement.childNodes[1].childNodes[0]);

        }
    }

    Component.onCompleted: {
        var x = new XMLHttpRequest;

        x.open("GET", "text.xml");

        // Test to the end
        x.onreadystatechange = function() {
            if (x.readyState == XMLHttpRequest.DONE) {

                dataOK = true;

                if (x.responseXML != null)
                    checkXML(x.responseXML);

            }
        }

        x.send()
    }
}




