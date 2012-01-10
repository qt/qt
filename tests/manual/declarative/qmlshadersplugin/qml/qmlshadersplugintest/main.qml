/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0

Item {
    id: main
    width: 360
    height: 640

    Rectangle {
        id: background
        visible: testCaseList.visible
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#EEEEEE" }
                 GradientStop { position: 1.0; color: "#AAAAAA" }
             }
    }

    Loader {
        id: testLoader
        width: parent.width
        height: parent.height
        visible: !testCaseList.visible
    }

    ListModel {
        id: testcaseModel
        ListElement { name: "TestEffectHierarchy.qml"; group: "Effect source property tests" }
        ListElement { name: "TestGrab.qml"; group: "Effect source property tests" }
        ListElement { name: "TestLive.qml"; group: "Effect source property tests" }
        ListElement { name: "TestImageFiltering.qml"; group: "Effect source property tests" }
        ListElement { name: "TestWrapRepeat.qml"; group: "Effect source property tests" }
        ListElement { name: "TestHorizontalWrap.qml"; group: "Effect source property tests" }
        ListElement { name: "TestVerticalWrap.qml"; group: "Effect source property tests" }
        ListElement { name: "TestTextureSize.qml"; group: "Effect source property tests" }
        ListElement { name: "TestItemMargins.qml"; group: "Effect source property tests" }
        ListElement { name: "TestEffectInsideAnotherEffect.qml"; group: "Effect source property tests" }
        ListElement { name: "TestItemMarginsWithTextureSize.qml"; group: "Effect source property tests" }
        ListElement { name: "TestHideOriginal.qml"; group: "Effect source property tests" }
        ListElement { name: "TestActive.qml"; group: "Effect item property tests" }
        ListElement { name: "TestBlending.qml"; group: "Effect item property tests" }
        ListElement { name: "TestBlendingModes.qml"; group: "Effect item property tests" }
        ListElement { name: "TestOpacity.qml"; group: "Effect item property tests" }
        ListElement { name: "TestFragmentShader.qml"; group: "Effect item property tests" }
        ListElement { name: "TestVertexShader.qml"; group: "Effect item property tests" }
        ListElement { name: "TestMeshResolution.qml"; group: "Effect item property tests" }
        ListElement { name: "TestRotation.qml"; group: "Shader effect transformation tests" }
        ListElement { name: "TestScale.qml"; group: "Shader effect transformation tests" }
        ListElement { name: "TestBasic.qml"; group: "Scenegraph effect tests" }
        ListElement { name: "TestOneSource.qml"; group: "Scenegraph effect tests" }
        ListElement { name: "TestTwiceOnSameSource.qml"; group: "Scenegraph effect tests" }
        ListElement { name: "TestTwoSources.qml"; group: "Scenegraph effect tests" }
    }

    Component {
        id: sectionHeading
        Rectangle {
            width: testCaseList.width
            height: 35
            color: "#00000000"

            Text {
                text: section
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                anchors.fill: parent
                anchors.leftMargin: 5
                font.bold: true
                style: Text.Raised
                styleColor: "white"
            }
        }
    }

    ListView {
        id: testCaseList

        property int hideTranslation: 0
        transform: Translate {
            x: testCaseList.hideTranslation
        }

        anchors.fill: parent
        anchors.topMargin: 10
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.bottomMargin: 10

        model: testcaseModel
        spacing: 3

        state: "testStopped"

        section.property: "group"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeading

        delegate: Rectangle {
            width: parent.width
            height:  50
            radius:  5
            border.width: 1
            border.color:  "#888888"
            color: delegateMouseArea.pressed ? "#AAAAFF" : "#FFFFFF"
            Text {
                id: delegateText;
                text: "  " + name
                width: parent.width
                height: parent.height
                font.pixelSize: 16
                verticalAlignment: Text.AlignVCenter
            }
            Text {
                id: delegateText2;
                text: ">  "
                width: parent.width
                height: parent.height
                font.pixelSize: 20
                smooth: true
                color: "gray"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
            }

            MouseArea {
                id: delegateMouseArea
                anchors.fill: parent;
                onClicked: {
                    testCaseList.state = "testRunning"
                    testLoader.source = name
                    console.log(name)
                }
            }
        }

        states: [
            State {
                name: "testRunning"
                PropertyChanges { target: testCaseList; visible: false; hideTranslation: -main.width  }
            },
            State {
                name: "testStopped"
                PropertyChanges { target: testCaseList; visible: true; hideTranslation: 0 }
            }
        ]

        transitions: [
            Transition {
            to: "testRunning"
                SequentialAnimation {
                    NumberAnimation { properties: "hideTranslation"; easing.type: Easing.InQuad; duration:  300 }
                    PropertyAction { target: testCaseList; property: "visible"; value: false }
                }
             },
            Transition {
                to: "testStopped"
                SequentialAnimation {
                    PropertyAction { target: testCaseList; property: "visible"; value: true }
                    NumberAnimation { properties: "hideTranslation"; easing.type: Easing.InQuad; duration:  300 }
                }
            }

        ]
    }

    Rectangle {
        visible: true
        anchors.bottom: main.bottom
        anchors.left: main.left
        anchors.right: main.right
        height: 40
        color: "#cc000000"
        Item {
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 20
            Image {
                source: "back.svg"
            }
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                if (testCaseList.visible){
                    Qt.quit()
                } else if (!testCaseList.state != "testStopped") {
                    testCaseList.state = "testStopped"
                    testLoader.source = ""
                }
            }
        }
    }
}

