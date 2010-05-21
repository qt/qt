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

import Qt.test 1.0

MyTypeObject {
    flagProperty: if(1) "FlagVal1 | FlagVal3"
    enumProperty: if(1) "EnumVal2"
    stringProperty: if(1) "Hello World!"
    uintProperty: if(1) 10
    intProperty: if(1) -19
    realProperty: if(1) 23.2
    doubleProperty: if(1) -19.7
    floatProperty: if(1) 8.5
    colorProperty: if(1) "red"
    dateProperty: if(1) "1982-11-25"
    timeProperty: if(1) "11:11:32" 
    dateTimeProperty: if(1) "2009-05-12T13:22:01"
    pointProperty: if(1) "99,13"
    pointFProperty: if(1) "-10.1,12.3"
    sizeProperty: if(1) "99x13"
    sizeFProperty: if(1) "0.1x0.2"
    rectProperty: if(1) "9,7,100x200"
    rectFProperty: if(1) "1000.1,-10.9,400x90.99"
    boolProperty: if(1) true
    variantProperty: if(1) "Hello World!"
    vectorProperty: if(1) "10,1,2.2"
    urlProperty: if(1) "main.qml"
}
