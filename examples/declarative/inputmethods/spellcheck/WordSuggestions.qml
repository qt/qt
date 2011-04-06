/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

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
