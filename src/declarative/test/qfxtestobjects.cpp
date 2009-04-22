/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include "qfxtestobjects.h"
#include <qml.h>
#include <QIODevice>


QT_BEGIN_NAMESPACE
QML_DECLARE_TYPE(TestObject);
QML_DEFINE_TYPE(TestObject,TestObject);
QML_DECLARE_TYPE(TestFrame);
QML_DEFINE_TYPE(TestFrame,TestFrame);
QML_DECLARE_TYPE(TestFullFrame);
QML_DEFINE_TYPE(TestFullFrame,TestFullFrame);
QML_DECLARE_TYPE(TestMouse);
QML_DEFINE_TYPE(TestMouse,TestMouse);
QML_DECLARE_TYPE(TestKey);
QML_DEFINE_TYPE(TestKey,TestKey);
QML_DECLARE_TYPE(TestLog);
QML_DEFINE_TYPE(TestLog,TestLog);

TestObject::TestObject(QObject *parent)
: QObject(parent), _time(-1)
{
}

int TestObject::time() const
{
    return _time;
}

void TestObject::setTime(int t)
{
    if(t == _time)
        return;
    _time = t;
    emit dataChanged();
}

void TestObject::save(QXmlStreamWriter *device)
{
    device->writeStartElement(QLatin1String("TestObject"));
    device->writeAttribute(QLatin1String("time"), QString::number(time()));
    device->writeEndElement();
}


TestFrame::TestFrame(QObject *parent)
: TestObject(parent)
{
}
    
QString TestFrame::hash() const
{
    return _hash;
}

void TestFrame::setHash(const QString &h)
{
    if(_hash == h)
        return;
    _hash = h;
    emit frameChanged();
}

void TestFrame::save(QXmlStreamWriter *device)
{
    device->writeStartElement(QLatin1String("TestFrame"));
    device->writeAttribute(QLatin1String("time"), QLatin1String(QByteArray::number(time())));
    device->writeAttribute(QLatin1String("hash"), hash());
    device->writeEndElement();
}

TestFullFrame::TestFullFrame(QObject *parent)
: TestObject(parent), _frameId(-1)
{
}

int TestFullFrame::frameId() const
{
    return _frameId;
}

void TestFullFrame::setFrameId(int id)
{
    if(id == _frameId)
        return;
    _frameId = id;
    emit frameChanged();
}

void TestFullFrame::save(QXmlStreamWriter *device)
{
    device->writeStartElement(QLatin1String("TestFullFrame"));
    device->writeAttribute(QLatin1String("time"), QLatin1String(QByteArray::number(time())));
    device->writeAttribute(QLatin1String("frameId"), QLatin1String(QByteArray::number(frameId())));
    device->writeEndElement();
}

TestMouse::TestMouse(QObject *parent)
: TestObject(parent), _type(-1), _button(-1), _buttons(-1)
{
}

int TestMouse::type() const
{
    return _type;
}

void TestMouse::setType(int t)
{
    if(_type == t)
        return;
    _type = t;
    emit mouseChanged();
}

int TestMouse::button() const
{
    return _button;
}

void TestMouse::setButton(int b)
{
    if(b == _button)
        return;
    _button = b;
    emit mouseChanged();
}

int TestMouse::buttons() const
{
    return _buttons;
}

void TestMouse::setButtons(int buttons)
{
    if(_buttons == buttons)
        return;
    _buttons = buttons;
    emit mouseChanged();
}

QPoint TestMouse::globalPos() const
{
    return _globalPos;
}

void TestMouse::setGlobalPos(const QPoint &g)
{
    if(_globalPos == g)
        return;
    _globalPos = g;
    emit mouseChanged();
}

QPoint TestMouse::pos() const
{
    return _pos;
}

void TestMouse::setPos(const QPoint &p)
{
    if(p == _pos)
        return;
    _pos = p;
    emit mouseChanged();
}

void TestMouse::save(QXmlStreamWriter *device)
{
    device->writeStartElement(QLatin1String("TestMouse"));
    device->writeAttribute(QLatin1String("time"), QString::number(time()));
    device->writeAttribute(QLatin1String("type"), QString::number(type()));
    device->writeAttribute(QLatin1String("button"), QString::number(button()));
    device->writeAttribute(QLatin1String("buttons"), QString::number(buttons()));
    device->writeAttribute(QLatin1String("globalPos"), QString::number(globalPos().x()) + QLatin1String(",") + QString::number(globalPos().y()));
    device->writeAttribute(QLatin1String("pos"), QString::number(pos().x()) + QLatin1String(",") + QString::number(pos().y()));
    device->writeEndElement();
}

TestKey::TestKey(QObject *parent)
: TestObject(parent), _type(-1), _modifiers(-1), _key(-1)
{
}

int TestKey::type() const
{
    return _type;
}

void TestKey::setType(int t)
{
    if(t == _type)
        return;
    _type = t;
    emit keyChanged();
}

int TestKey::modifiers() const
{
    return _modifiers;
}

void TestKey::setModifiers(int m)
{
    if(m == _modifiers)
        return;
    _modifiers = m;
    emit keyChanged();
}

QString TestKey::text() const
{
    return _text;
}

void TestKey::setText(const QString &t)
{
    if(_text == t)
        return;
    _text = t;
    emit keyChanged();
}

int TestKey::key() const
{
    return _key;
}

void TestKey::setKey(int k)
{
    if(_key == k)
        return;
    _key = k;
    emit keyChanged();
}

void TestKey::save(QXmlStreamWriter *device)
{
    device->writeStartElement(QLatin1String("TestKey"));
    device->writeAttribute(QLatin1String("time"), QString::number(time()));
    device->writeAttribute(QLatin1String("type"), QString::number(type()));
    device->writeAttribute(QLatin1String("modifiers"), QString::number(modifiers()));
    device->writeAttribute(QLatin1String("key"), QString::number(key()));
    if(key() != Qt::Key_Escape)
        device->writeAttribute(QLatin1String("text"), text());
    device->writeEndElement();
}

TestLog::TestLog(QObject *parent)
: QObject(parent), _current(0)
{
}

QList<TestObject *> *TestLog::qmlActions()
{
    return &_actions;
}

QList<TestObject *> &TestLog::actions()
{
    return _actions;
}

bool lessThan(TestObject *lhs, TestObject *rhs)
{
    return lhs->time() < rhs->time();
}

void TestLog::save(QIODevice *device)
{
    // Order correctly
    qStableSort(_actions.begin(), _actions.end(), lessThan);

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument(QLatin1String("1.0"));
    writer.writeStartElement(QLatin1String("TestLog"));
    for(int ii = 0; ii < _actions.count(); ++ii)
        _actions.at(ii)->save(&writer);
    writer.writeEndElement();
    writer.writeEndDocument();
}

TestObject *TestLog::next()
{
    if(atEnd())
        return 0;
    TestObject *rv = _actions.at(_current);
    _current++;
    return rv;
}

bool TestLog::atEnd() const
{
    if(_current >= _actions.count())
        return true;
    else
        return false;
}

int TestLog::current() const
{
    return _current;
}

QT_END_NAMESPACE
