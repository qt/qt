/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7

FocusScope {
    id: wrapper
    Column {
        anchors.centerIn: parent
        spacing: 20
        Column{
            spacing: 4
            Text {
                text: "Posted by:"
                font.pixelSize: 16; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                horizontalAlignment: Qt.AlignRight
            }
            Input{
                id: fromIn
                KeyNavigation.backtab: searchbutton
                KeyNavigation.tab:toIn
                onAccepted:searchbutton.doSearch();
                focus: true
            }
            Text {
                text: "In reply to:"
                font.pixelSize: 16; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                horizontalAlignment: Qt.AlignRight
            }
            Input{
                id: toIn
                KeyNavigation.backtab: fromIn
                KeyNavigation.tab:phraseIn
                onAccepted:searchbutton.doSearch();
            }
            Text {
                text: "Search phrase:"
                font.pixelSize: 16; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                horizontalAlignment: Qt.AlignRight
            }
            Input{
                id: phraseIn
                KeyNavigation.backtab: toIn
                KeyNavigation.tab:searchbutton
                onAccepted:searchbutton.doSearch();
                text: "Qt Quick"
            }
        }
        Button {
            width: 100
            height: 32
            id: searchbutton
            keyUsing: true;
            opacity: 1
            text: "Search"
            KeyNavigation.tab: fromIn
            Keys.onReturnPressed: searchbutton.doSearch();
            Keys.onEnterPressed: searchbutton.doSearch();
            Keys.onSelectPressed: searchbutton.doSearch();
            Keys.onSpacePressed: searchbutton.doSearch();
            onClicked: searchbutton.doSearch();

            function doSearch() {
                // Search ! allowed
                if (wrapper.state=="invalidinput")
                    return;

                rssModel.from=fromIn.text;
                rssModel.to= toIn.text;
                rssModel.phrase = phraseIn.text;
                screen.focus = true;
                screen.state = ""
            }
        }
    }
    states:
    State {
        name: "invalidinput"
        when: fromIn.text=="" && toIn.text=="" && phraseIn.text==""
        PropertyChanges { target: searchbutton ; opacity: 0.6 ; }
    }
    transitions:
    Transition {
        NumberAnimation { target: searchbutton; property: "opacity"; duration: 200 }
    }
}
