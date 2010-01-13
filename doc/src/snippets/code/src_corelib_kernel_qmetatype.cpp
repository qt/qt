/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
struct MyStruct
{
    int i;
    ...
};

Q_DECLARE_METATYPE(MyStruct)
//! [0]


//! [1]
namespace MyNamespace
{
    ...
}

Q_DECLARE_METATYPE(MyNamespace::MyStruct)
//! [1]


//! [2]
MyStruct s;
QVariant var;
var.setValue(s); // copy s into the variant

...

// retrieve the value
MyStruct s2 = var.value<MyStruct>();
//! [2]


//! [3]
int id = QMetaType::type("MyClass");
if (id == 0) {
    void *myClassPtr = QMetaType::construct(id);
    ...
    QMetaType::destroy(id, myClassPtr);
    myClassPtr = 0;
}
//! [3]


//! [4]
qRegisterMetaType<MyClass>("MyClass");
//! [4]


//! [5]
qRegisterMetaTypeStreamOperators<MyClass>("MyClass");
//! [5]


//! [6]
QDataStream &operator<<(QDataStream &out, const MyClass &myObj);
QDataStream &operator>>(QDataStream &in, MyClass &myObj);
//! [6]


//! [7]
int id = qRegisterMetaType<MyStruct>();
//! [7]


//! [8]
int id = qMetaTypeId<QString>();    // id is now QMetaType::QString
id = qMetaTypeId<MyStruct>();       // compile error if MyStruct not declared
//! [8]
