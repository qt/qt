import Qt 4.6

Rectangle {
    width: 800; height: 600

    MouseRegion {
        anchors.fill: parent
        onClicked: { 

                var doc = new XMLHttpRequest();
                doc.onreadystatechange = function() {
                    if (doc.readyState == XMLHttpRequest.DONE) {
    
                        var a = doc.responseXML.documentElement;
                        for (var ii = 0; ii < a.childNodes.length; ++ii) {
                            print (a.childNodes[ii].nodeName);
                        }

                    }
                }

                doc.open("GET", "test.xml");
                doc.send();
        }
    }
}
