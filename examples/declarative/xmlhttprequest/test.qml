import Qt 4.7

Rectangle {
    width: 800; height: 600

    MouseArea {
        anchors.fill: parent

        onClicked: { 
            var doc = new XMLHttpRequest();
            doc.onreadystatechange = function() {
                if (doc.readyState == XMLHttpRequest.HEADERS_RECEIVED) {
                    console.log("Headers -->");
                    console.log(doc.getAllResponseHeaders ());
                    console.log("Last modified -->");
                    console.log(doc.getResponseHeader ("Last-Modified"));
                }
                else if (doc.readyState == XMLHttpRequest.DONE) {

                    var a = doc.responseXML.documentElement;
                    for (var ii = 0; ii < a.childNodes.length; ++ii) {
                        console.log(a.childNodes[ii].nodeName);
                    }
                    console.log("Headers -->");
                    console.log(doc.getAllResponseHeaders ());
                    console.log("Last modified -->");
                    console.log(doc.getResponseHeader ("Last-Modified"));

                }
            }

            doc.open("GET", "test.xml");
            doc.send();
        }
    }
}
