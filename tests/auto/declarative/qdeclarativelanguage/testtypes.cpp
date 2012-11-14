/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "testtypes.h"

void registerTypes()
{
    qmlRegisterInterface<MyInterface>("MyInterface");
    qmlRegisterType<MyQmlObject>("Test",1,0,"MyQmlObject");
    qmlRegisterType<MyTypeObject>("Test",1,0,"MyTypeObject");
    qmlRegisterType<MyContainer>("Test",1,0,"MyContainer");
    qmlRegisterType<MyPropertyValueSource>("Test",1,0,"MyPropertyValueSource");
    qmlRegisterType<MyDotPropertyObject>("Test",1,0,"MyDotPropertyObject");
    qmlRegisterType<MyNamespace::MyNamespacedType>("Test",1,0,"MyNamespacedType");
    qmlRegisterType<MyNamespace::MySecondNamespacedType>("Test",1,0,"MySecondNamespacedType");
    qmlRegisterType<MyParserStatus>("Test",1,0,"MyParserStatus");
    qmlRegisterType<MyGroupedObject>();
    qmlRegisterType<MyRevisionedClass>("Test",1,0,"MyRevisionedClass");
    qmlRegisterType<MyRevisionedClass,1>("Test",1,1,"MyRevisionedClass");
    qmlRegisterType<MyRevisionedIllegalOverload>("Test",1,0,"MyRevisionedIllegalOverload");
    qmlRegisterType<MyRevisionedLegalOverload>("Test",1,0,"MyRevisionedLegalOverload");

    // Register the uncreatable base class
    qmlRegisterRevision<MyRevisionedBaseClassRegistered,1>("Test",1,1);
    // MyRevisionedSubclass 1.0 uses MyRevisionedClass revision 0
    qmlRegisterType<MyRevisionedSubclass>("Test",1,0,"MyRevisionedSubclass");
    // MyRevisionedSubclass 1.1 uses MyRevisionedClass revision 1
    qmlRegisterType<MyRevisionedSubclass,1>("Test",1,1,"MyRevisionedSubclass");

    // Only version 1.0, but its super class is registered in version 1.1 also
    qmlRegisterType<MySubclass>("Test",1,0,"MySubclass");

    qmlRegisterCustomType<MyCustomParserType>("Test", 1, 0, "MyCustomParserType", new MyCustomParserTypeParser);

    qmlRegisterTypeNotAvailable("Test",1,0,"UnavailableType", "UnavailableType is unavailable for testing");
}

QVariant myCustomVariantTypeConverter(const QString &data)
{
    MyCustomVariantType rv;
    rv.a = data.toInt();
    return QVariant::fromValue(rv);
}

