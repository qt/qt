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

Rectangle {
    id: s; width: 800; height: 1000; color: "lightsteelblue"
    property string text: "The quick brown fox jumps over the lazy dog."

    Column {
        spacing: 10
        Text {
            text: s.text
        }
        Text {
            text: s.text; font.pixelSize: 18
        }
        Text {
            text: s.text; font.pointSize: 25
        }
        Text {
            text: s.text; color: "red"; smooth: true
        }
        Text {
            text: s.text; font.capitalization: "AllUppercase"
        }
        Text {
            text: s.text; font.underline: true
        }
        Text {
            text: s.text; font.overline: true; smooth: true
        }
        Text {
            text: s.text; font.strikeout: true
        }
        Text {
            text: s.text; font.underline: true; font.overline: true; font.strikeout: true
        }
        Text {
            text: s.text; font.letterSpacing: 200
        }
        Text {
            text: s.text; font.underline: true; font.letterSpacing: 200; font.capitalization: "AllUppercase"; color: "blue"
        }
        Text {
            text: s.text; font.overline: true; font.wordSpacing: 25; font.capitalization: "Capitalize"; color: "green"
        }
        Text {
            text: s.text; font.pixelSize: 18; style: Text.Outline; styleColor: "white"
        }
        Text {
            text: s.text; font.pixelSize: 18; style: Text.Sunken; styleColor: "gray"
        }
        Text {
            text: s.text; font.pixelSize: 18; style: Text.Raised; styleColor: "yellow"
        }
        Text {
            text: s.text; horizontalAlignment: Text.AlignLeft; width: 800
        }
        Text {
            text: s.text; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; width: 800; height: 20
        }
        Text {
            text: s.text; horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignBottom; width: 800; height: 20
        }
        Text {
            text: s.text; font.pixelSize: 18; style: Text.Outline; styleColor: "white"; wrapMode: Text.WordWrap; width: 200
        }
        Text {
            text: s.text; elide: Text.ElideLeft; width: 200
        }
        Text {
            text: s.text; elide: Text.ElideMiddle; width: 200
        }
        Text {
            text: s.text; elide: Text.ElideRight; width: 200
        }
        Text {
            text: s.text; elide: Text.ElideLeft; width: 200; wrapMode: Text.WordWrap
        }
        Text {
            text: s.text; elide: Text.ElideMiddle; width: 200; wrapMode: Text.WordWrap
        }
        Text {
            text: s.text; elide: Text.ElideRight; width: 200; wrapMode: Text.WordWrap
        }
        Text {
            text: s.text + " thisisaverylongstringwithnospaces"; width: 150; wrapMode: Text.WrapAnywhere
        }
        Text {
            text: s.text + " thisisaverylongstringwithnospaces"; width: 150; wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}
