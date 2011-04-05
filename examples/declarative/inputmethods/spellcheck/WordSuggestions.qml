import QtQuick 2.0
import Qt.labs.inputcontext 1.0 as InputContext

ListView {
    property int globalX: InputContext.microFocus.x + ((InputContext.microFocus.width - width) / 2)
    property int globalY: InputContext.microFocus.y + InputContext.microFocus.height

    x: parent.mapToItem(null, globalX, globalY).x
    y: parent.mapToItem(null, globalX, globalY).y

    visible: suggestionModel.count > 0

    width: 200
    height: 70

    InputContext.KeyFilter {
        onPressed: event.accepted = filterKeyPress(event.key, event.text)
    }

    InputContext.MouseHandler {
        onPressed: {
            if (cursor < 0 || cursor >= InputContext.preeditText.length)
                InputContext.commit()
        }
    }

    model: XmlListModel {
        id: suggestionModel

        query: "/query/results/s:suggestion"
        namespaceDeclarations: "declare namespace s=\"http://www.inktomi.com/\";"
        source: InputContext.preeditText.length > 4 ? "http://query.yahooapis.com/v1/public/yql?q=select * from search.spelling where query=\"" + InputContext.preeditText + "\"" : ""

        XmlRole { name: "suggestion"; query: "string()" }
    }

    delegate: Rectangle {
        radius: 2
        color: "lightsteelblue"

        anchors.horizontalCenter: parent.horizontalCenter

        width: suggestionText.implicitWidth + 2
        height: suggestionText.implicitHeight + 2

        Text {
            id: suggestionText

            font: InputContext.font
            text: suggestion

            anchors.fill: parent
            anchors.margins: 1
        }
        MouseArea {
            anchors.fill: parent
            onClicked: InputContext.commit(suggestion)
        }
    }
}
