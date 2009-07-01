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

#ifndef _QFXTESTOBJECTS_H_
#define _QFXTESTOBJECTS_H_

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QList>
#include <QtCore/QTextStream>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QIODevice;
class TestObject : public QObject
{
Q_OBJECT
public:
    TestObject(QObject * = 0);

    Q_PROPERTY(int time READ time WRITE setTime NOTIFY dataChanged)
    int time() const;
    void setTime(int);

    virtual void save(QTextStream &, int pad);
Q_SIGNALS:
    void dataChanged();

private:
    int _time;
};

class TestFrame : public TestObject
{
Q_OBJECT
public:
    TestFrame(QObject * = 0);
    
    Q_PROPERTY(QString hash READ hash WRITE setHash NOTIFY frameChanged)
    QString hash() const;
    void setHash(const QString &);

    virtual void save(QTextStream &, int pad);
Q_SIGNALS:
    void frameChanged();

private:
    QString _hash;
};

class TestFullFrame : public TestObject
{
Q_OBJECT
public:
    TestFullFrame(QObject * = 0);

    Q_PROPERTY(int frameId READ frameId WRITE setFrameId NOTIFY frameChanged)
    int frameId() const;
    void setFrameId(int);

    virtual void save(QTextStream &, int pad);
Q_SIGNALS:
    void frameChanged();

private:
    int _frameId;
};

class TestMouse : public TestObject
{
Q_OBJECT
public:
    TestMouse(QObject * = 0);

    Q_PROPERTY(int type READ type WRITE setType NOTIFY mouseChanged)
    int type() const;
    void setType(int);

    Q_PROPERTY(int button READ button WRITE setButton NOTIFY mouseChanged)
    int button() const;
    void setButton(int);

    Q_PROPERTY(int buttons READ buttons WRITE setButtons NOTIFY mouseChanged)
    int buttons() const;
    void setButtons(int);

    Q_PROPERTY(QPoint globalPos READ globalPos WRITE setGlobalPos NOTIFY mouseChanged)
    QPoint globalPos() const;
    void setGlobalPos(const QPoint &);

    Q_PROPERTY(QPoint pos READ pos WRITE setPos NOTIFY mouseChanged)
    QPoint pos() const;
    void setPos(const QPoint &);

    virtual void save(QTextStream &, int pad);

Q_SIGNALS:
    void mouseChanged();

private:
    int _type;
    int _button;
    int _buttons;
    QPoint _globalPos;
    QPoint _pos;
};

class TestKey : public TestObject
{
Q_OBJECT
public:
    TestKey(QObject * = 0);

    Q_PROPERTY(int type READ type WRITE setType NOTIFY keyChanged)
    int type() const;
    void setType(int);

    Q_PROPERTY(int modifiers READ modifiers WRITE setModifiers NOTIFY keyChanged)
    int modifiers() const;
    void setModifiers(int);

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY keyChanged)
    QString text() const;
    void setText(const QString &);

    Q_PROPERTY(int key READ key WRITE setKey NOTIFY keyChanged)
    int key() const;
    void setKey(int);

    virtual void save(QTextStream &, int pad);

Q_SIGNALS:
    void keyChanged();

private:
    int _type;
    int _modifiers;
    int _key;
    QString _text;
};

class TestLog : public QObject
{
Q_OBJECT
public:
    TestLog(QObject * = 0);

    Q_CLASSINFO("DefaultProperty", "actions")
    Q_PROPERTY(QList<TestObject *> *actions READ qmlActions)
    QList<TestObject *> *qmlActions();

    QList<TestObject *> &actions();

    int current() const;
    void save(QIODevice *);

    TestObject *next();
    bool atEnd() const;

private:
    int _current;
    QList<TestObject *> _actions;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // _QFXTESTOBJECTS_H_
