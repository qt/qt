/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7

Item {
    id:lineedit
    property alias text: textInp.text

    width: textInp.width + 11 
    height: 13 + 11 

    Rectangle{
        color: 'lightsteelblue'
        anchors.fill: parent
    }
    clip: true
    Component.onCompleted: textInp.cursorPosition = 0;
    TextInput{
        id:textInp
        cursorDelegate: Item{
            Rectangle{
                visible: parent.parent.focus
                color: "#009BCE"
                height: 13
                width: 2
                y: 1
            }
        }
        property int leftMargin: 6 
        property int rightMargin: 6 
        x: leftMargin
        y: 5
        //Below function implements all scrolling logic
        onCursorPositionChanged: {
            if(cursorRect.x < leftMargin - textInp.x){//Cursor went off the front
                textInp.x = leftMargin - Math.max(0, cursorRect.x);
            }else if(cursorRect.x > parent.width - leftMargin - rightMargin - textInp.x){//Cusor went off the end
                textInp.x = leftMargin - Math.max(0, cursorRect.x - (parent.width - leftMargin - rightMargin));
            }
        }

        text:""
        horizontalAlignment: TextInput.AlignLeft
        font.pixelSize:15
    }
    MouseArea{
        //Implements all line edit mouse handling
        id: mainMouseArea
        anchors.fill: parent;
        function translateX(x){
            return x - textInp.x
        }
        onPressed: {
            textInp.focus = true;
            textInp.cursorPosition = textInp.xToPosition(translateX(mouse.x));
        }
        onPositionChanged: {
            textInp.moveCursorSelection(textInp.xToPosition(translateX(mouse.x)));
        }
        onReleased: {
        }
        onDoubleClicked: {
            textInp.selectionStart=0;
            textInp.selectionEnd=textInp.text.length;
        }
        z: textInp.z + 1
    }

}
