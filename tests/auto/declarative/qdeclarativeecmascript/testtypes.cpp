/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include "testtypes.h"
#include <QWidget>
#include <QPlainTextEdit>

class BaseExtensionObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int baseExtendedProperty READ extendedProperty WRITE setExtendedProperty NOTIFY extendedPropertyChanged)
public:
    BaseExtensionObject(QObject *parent) : QObject(parent), m_value(0) {}

    int extendedProperty() const { return m_value; }
    void setExtendedProperty(int v) { m_value = v; emit extendedPropertyChanged(); }

signals:
    void extendedPropertyChanged();
private:
    int m_value;
};

class ExtensionObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int extendedProperty READ extendedProperty WRITE setExtendedProperty NOTIFY extendedPropertyChanged)
public:
    ExtensionObject(QObject *parent) : QObject(parent), m_value(0) {}

    int extendedProperty() const { return m_value; }
    void setExtendedProperty(int v) { m_value = v; emit extendedPropertyChanged(); }

signals:
    void extendedPropertyChanged();
private:
    int m_value;
};

class DefaultPropertyExtensionObject : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "firstProperty")
public:
    DefaultPropertyExtensionObject(QObject *parent) : QObject(parent) {}
};

class QWidgetDeclarativeUI : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)

signals:
    void widthChanged();

public:
    QWidgetDeclarativeUI(QObject *other) : QObject(other) { }

public:
    int width() const { return 0; }
    void setWidth(int) { }
};


void registerTypes()
{
    qmlRegisterType<MyQmlObject>("Qt.test", 1,0, "MyQmlObjectAlias");
    qmlRegisterType<MyQmlObject>("Qt.test", 1,0, "MyQmlObject");
    qmlRegisterType<MyDeferredObject>("Qt.test", 1,0, "MyDeferredObject");
    qmlRegisterType<MyQmlContainer>("Qt.test", 1,0, "MyQmlContainer");
    qmlRegisterExtendedType<MyBaseExtendedObject, BaseExtensionObject>("Qt.test", 1,0, "MyBaseExtendedObject");
    qmlRegisterExtendedType<MyExtendedObject, ExtensionObject>("Qt.test", 1,0, "MyExtendedObject");
    qmlRegisterType<MyTypeObject>("Qt.test", 1,0, "MyTypeObject");
    qmlRegisterType<MyDerivedObject>("Qt.test", 1,0, "MyDerivedObject");
    qmlRegisterType<NumberAssignment>("Qt.test", 1,0, "NumberAssignment");
    qmlRegisterExtendedType<DefaultPropertyExtendedObject, DefaultPropertyExtensionObject>("Qt.test", 1,0, "DefaultPropertyExtendedObject");
    qmlRegisterType<OverrideDefaultPropertyObject>("Qt.test", 1,0, "OverrideDefaultPropertyObject");

    qmlRegisterExtendedType<QWidget,QWidgetDeclarativeUI>("Qt.test",1,0,"QWidget");
    qmlRegisterType<QPlainTextEdit>("Qt.test",1,0,"QPlainTextEdit");
}

#include "testtypes.moc"
