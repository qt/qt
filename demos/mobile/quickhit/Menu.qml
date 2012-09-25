/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import "Game.js" as GameScript


Rectangle {
    id: menu

    Text {
        id: title
        smooth: true
        anchors.bottom: menu.top
        anchors.horizontalCenter: menu.horizontalCenter
        color: "red"
        font.pixelSize:  gameArea.width / 12
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
    }

    border.color: "black"
    border.width: 2
    radius: 8
    smooth: true
    opacity: 0 // transparent by default

    color: "black"
//    gradient: Gradient {
//        GradientStop { position: 0.0; color: "white" }
//        GradientStop { position: 1.0; color: "black" }
//    }

    // Signals
    signal levelSelected(int levelIndex)
    signal resumeSelected()

    // Show level menu
    function showLevelMenu() {
        menuData.clear()
        title.text = "Select Your Level"
        var plugins = GameEngine.pluginList()
        for (var i = 0; i < plugins.length; i++) {
            menuData.append({"name": plugins[i],"type":"0"})
        }
        menuData.append({"name": "About","type":"4"})
        menuData.append({"name": "Exit","type":"1"})
        menu.opacity = 0.8
        listView.focus = true
    }

    function showPauseMenu() {
        menuData.clear()
        title.text = "Resume level?"
        menuData.append({"name": "Resume","type":"2"})
        menuData.append({"name": "End Game","type":"3"})
        menu.opacity = 0.8
        listView.focus = true
    }

    // Hide menu
    function hideMenu() {
        menu.opacity = 0
        listView.focus = false
    }

    function menuItemSelected(index, type) {
        switch (type) {
        case "0": {
                // Level selection
                menu.levelSelected(index)
                break;
            }
        case "1": {
                // Exit
                Qt.quit()
                break;
            }
        case "2": {
                // Resume
                menu.resumeSelected()
                break;
            }
        case "3": {
                // End Game
                gameArea.endGame()
                break;
            }
        case "4": {
                // Show about
                gameArea.messageBox.showInfoMessage()
                break;
            }
        default: {
                break;
            }
        }
    }

    ListModel {
        id: menuData
    }

    ListView {
        id: listView
        clip: true
        anchors.fill: parent
        keyNavigationWraps: true

        function doSelect() {
            menu.menuItemSelected(listView.currentIndex,menuData.get(listView.currentIndex).type)
        }

        model: menuData

        delegate: MenuItem {
            id: menuItem
            selectedItem: menuItem.ListView.isCurrentItem ? true : false
            onItemSelected: {
                listView.currentIndex = model.index
                listView.doSelect()
            }

        }
    }
}
