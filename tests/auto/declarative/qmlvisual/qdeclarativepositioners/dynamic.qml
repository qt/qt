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
    width: 400; height: 400;
    property int step: 0
    function tick()
    {
        step++;
        if(step == 1){
            row1.destroy(); //Not dynamically created, so is this valid?
        }else if(step == 2){
            r2a.destroy();
        }else if(step == 3){
            r2b.destroy();
        }else if(step == 4){
            r2c.destroy();
        }else if(step == 5){
            r3a.parent = row2;
        }else if(step == 6){
            r3b.parent = row2;
        }else if(step == 7){
            r3c.parent = row2;
        }else if(step == 8){
            row3.destroy();
        }else{
            repeater.model++;
        }
    }

    //Tests base positioner functionality, so just using row
    Row{
        id: row1
        Rectangle{id: r1a; width:20; height:20; color: "red"}
        Rectangle{id: r1b; width:20; height:20; color: "green"}
        Rectangle{id: r1c; width:20; height:20; color: "blue"}
    }
    Row{
        y:20
        id: row2
        move: Transition{NumberAnimation{properties:"x"}}
        Repeater{ 
            id: repeater 
            model: 0; 
            delegate: Component{ Rectangle { color: "yellow"; width:20; height:20;}}
        }
        Rectangle{id: r2a; width:20; height:20; color: "red"}
        Rectangle{id: r2b; width:20; height:20; color: "green"}
        Rectangle{id: r2c; width:20; height:20; color: "blue"}
    }
    Row{
        move: Transition{NumberAnimation{properties:"x"}}
        y:40
        id: row3
        Rectangle{id: r3a; width:20; height:20; color: "red"}
        Rectangle{id: r3b; width:20; height:20; color: "green"}
        Rectangle{id: r3c; width:20; height:20; color: "blue"}
    }
    Timer{
        interval: 500;
        running: true;
        repeat: true;
        onTriggered: tick();
    }
}

