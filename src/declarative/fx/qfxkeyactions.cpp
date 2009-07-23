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

#include "qfxkeyactions.h"
#include <qmlexpression.h>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,KeyActions,QFxKeyActions)

class QFxKeyActionsPrivate
{
public:
    QFxKeyActionsPrivate();

    bool enabled;

    uint keys1;
    uint keys2;

    QHash<Qt::Key, QString> actions;

    bool key(Qt::Key) const;
    QString action(Qt::Key) const;
    void setKey(Qt::Key, bool = true);

    bool testBit(int) const;
    void setBit(int, bool);

    int keyToBit(Qt::Key) const;

    QString keyExpr(Qt::Key) const;
    void setKeyExpr(Qt::Key, const QString &);
};

QFxKeyActionsPrivate::QFxKeyActionsPrivate()
: enabled(true), keys1(0), keys2(0)
{
}

int QFxKeyActionsPrivate::keyToBit(Qt::Key k) const
{
    if (k >= Qt::Key_A && k <= Qt::Key_Z ) {
        return k - Qt::Key_A;
    } else if (k >= Qt::Key_Left && k <= Qt::Key_Down) {
        return 26 + k - Qt::Key_Left;
    } else if (k >= Qt::Key_0 && k <= Qt::Key_9) {
        return 30 + k - Qt::Key_0;
    } else if (k >= Qt::Key_Context1 && k <= Qt::Key_Flip) {
        return 40 + k - Qt::Key_Context1;
    } else if (k >= Qt::Key_Select && k <= Qt::Key_No) {
        return 47 + k - Qt::Key_Select;
    } else {
        const int start = 50;
        switch(k) {
        case Qt::Key_Escape:
            return start + 0;
        case Qt::Key_Return:
            return start + 1;
        case Qt::Key_Enter:
            return start + 2;
        case Qt::Key_Delete:
            return start + 3;
        case Qt::Key_Space:
            return start + 4;
        case Qt::Key_Back:
            return start + 5;
        case Qt::Key_unknown:
            return start + 6;
        case Qt::Key_Asterisk:
            return start + 7;
        default:
            return -1;
        }
    }
}

bool QFxKeyActionsPrivate::key(Qt::Key k) const
{
    int b = keyToBit(k);
    bool rv = testBit(b);
    if (!rv && k != Qt::Key_Shift)
        rv = testBit(keyToBit(Qt::Key_unknown));
    return rv;
}

QString QFxKeyActionsPrivate::action(Qt::Key k) const
{
    int b = keyToBit(k);
    if (b != -1 && testBit(b))
        return actions.value(k);
    else
        return actions.value(Qt::Key_unknown);
}

void QFxKeyActionsPrivate::setKey(Qt::Key k, bool v)
{
    int b = keyToBit(k);
    if (b == -1)
        return;

    setBit(b, v);
}

bool QFxKeyActionsPrivate::testBit(int b) const
{
    if (b < 0)
        return false;

    if (b < 32)
        return keys1 & (1 << b);
    else
        return keys2 & (1 << (b - 32));
}

void QFxKeyActionsPrivate::setBit(int b, bool v) 
{
    if (v) {
        if (b < 32)
            keys1 |= (1 << b);
        else
            keys2 |= (1 << (b - 32));
    } else {
        if (b < 32)
            keys1 &= ~(1 << b);
        else
            keys2 &= ~(1 << (b - 32));
    }
}


/*!
    \qmlclass KeyActions
    \brief The KeyActions item enables simple key handling.
    \inherits Item

    KeyActions is typically used in basic key handling scenarios where writing
    JavaScript key handling routines would be unnecessarily complicated.  The
    KeyActions item has a collection of properties that correspond to a
    selection of common keys.  When a given key is pressed, the item executes
    the action script assigned to the matching property.  If no action has
    been set the KeyActions item does nothing.

    To receive (and susequently respond to) key presses, the KeyActions class 
    must be in the current focus chain, just like any other item.

    For basic mouse handling, see \l MouseRegion.

    KeyActions is an invisible item: it is never painted.  
*/
QFxKeyActions::QFxKeyActions(QFxItem *parent)
: QFxItem(parent), d(new QFxKeyActionsPrivate)
{
}

QFxKeyActions::~QFxKeyActions()
{
    delete d;
}

QString QFxKeyActionsPrivate::keyExpr(Qt::Key k) const
{
    if (key(k))
        return actions.value(k);
    else
        return QString();
}

void QFxKeyActionsPrivate::setKeyExpr(Qt::Key k, const QString &expr)
{
    if (expr.isEmpty()) {
        if (key(k)) {
            actions.remove(k);
            setKey(k, false);
        }
    } else {
        actions.insert(k, expr);
        setKey(k);
    }
}

/*!
    \qmlproperty bool KeyActions::enabled

    Enables or disables KeyActions' key handling.  When not enabled, the 
    KeyActions instance does not respond to any key presses.  The item is
    enabled by default.
*/
bool QFxKeyActions::enabled() const
{
    return d->enabled;
}

void QFxKeyActions::setEnabled(bool e)
{
    if (d->enabled == e)
        return;

    d->enabled = e;
    emit enabledChanged();
}

/*!
    \qmlproperty string KeyActions::keyA...keyZ

    The action to take for the given letter.

    The following example sets actions for the 'c' and 'x' keys.
    \qml
    KeyActions { keyC: "print('c is for cookie')"; keyX: "print('I like cookies')" }
    \endqml
*/
QString QFxKeyActions::key_A() const
{
    return d->keyExpr(Qt::Key_A);
}

void QFxKeyActions::setKey_A(const QString &s)
{
    d->setKeyExpr(Qt::Key_A, s);
}

QString QFxKeyActions::key_B() const
{
    return d->keyExpr(Qt::Key_B);
}

void QFxKeyActions::setKey_B(const QString &s)
{
    d->setKeyExpr(Qt::Key_B, s);
}

QString QFxKeyActions::key_C() const
{
    return d->keyExpr(Qt::Key_C);
}

void QFxKeyActions::setKey_C(const QString &s)
{
    d->setKeyExpr(Qt::Key_C, s);
}

QString QFxKeyActions::key_D() const
{
    return d->keyExpr(Qt::Key_D);
}

void QFxKeyActions::setKey_D(const QString &s)
{
    d->setKeyExpr(Qt::Key_D, s);
}

QString QFxKeyActions::key_E() const
{
    return d->keyExpr(Qt::Key_E);
}

void QFxKeyActions::setKey_E(const QString &s)
{
    d->setKeyExpr(Qt::Key_E, s);
}

QString QFxKeyActions::key_F() const
{
    return d->keyExpr(Qt::Key_F);
}

void QFxKeyActions::setKey_F(const QString &s)
{
    d->setKeyExpr(Qt::Key_F, s);
}

QString QFxKeyActions::key_G() const
{
    return d->keyExpr(Qt::Key_G);
}

void QFxKeyActions::setKey_G(const QString &s)
{
    d->setKeyExpr(Qt::Key_G, s);
}

QString QFxKeyActions::key_H() const
{
    return d->keyExpr(Qt::Key_H);
}

void QFxKeyActions::setKey_H(const QString &s)
{
    d->setKeyExpr(Qt::Key_H, s);
}

QString QFxKeyActions::key_I() const
{
    return d->keyExpr(Qt::Key_I);
}

void QFxKeyActions::setKey_I(const QString &s)
{
    d->setKeyExpr(Qt::Key_I, s);
}

QString QFxKeyActions::key_J() const
{
    return d->keyExpr(Qt::Key_J);
}

void QFxKeyActions::setKey_J(const QString &s)
{
    d->setKeyExpr(Qt::Key_J, s);
}

QString QFxKeyActions::key_K() const
{
    return d->keyExpr(Qt::Key_K);
}

void QFxKeyActions::setKey_K(const QString &s)
{
    d->setKeyExpr(Qt::Key_K, s);
}

QString QFxKeyActions::key_L() const
{
    return d->keyExpr(Qt::Key_L);
}

void QFxKeyActions::setKey_L(const QString &s)
{
    d->setKeyExpr(Qt::Key_L, s);
}

QString QFxKeyActions::key_M() const
{
    return d->keyExpr(Qt::Key_M);
}

void QFxKeyActions::setKey_M(const QString &s)
{
    d->setKeyExpr(Qt::Key_M, s);
}

QString QFxKeyActions::key_N() const
{
    return d->keyExpr(Qt::Key_N);
}

void QFxKeyActions::setKey_N(const QString &s)
{
    d->setKeyExpr(Qt::Key_N, s);
}

QString QFxKeyActions::key_O() const
{
    return d->keyExpr(Qt::Key_O);
}

void QFxKeyActions::setKey_O(const QString &s)
{
    d->setKeyExpr(Qt::Key_O, s);
}

QString QFxKeyActions::key_P() const
{
    return d->keyExpr(Qt::Key_P);
}

void QFxKeyActions::setKey_P(const QString &s)
{
    d->setKeyExpr(Qt::Key_P, s);
}

QString QFxKeyActions::key_Q() const
{
    return d->keyExpr(Qt::Key_Q);
}

void QFxKeyActions::setKey_Q(const QString &s)
{
    d->setKeyExpr(Qt::Key_Q, s);
}

QString QFxKeyActions::key_R() const
{
    return d->keyExpr(Qt::Key_R);
}

void QFxKeyActions::setKey_R(const QString &s)
{
    d->setKeyExpr(Qt::Key_R, s);
}

QString QFxKeyActions::key_S() const
{
    return d->keyExpr(Qt::Key_S);
}

void QFxKeyActions::setKey_S(const QString &s)
{
    d->setKeyExpr(Qt::Key_S, s);
}

QString QFxKeyActions::key_T() const
{
    return d->keyExpr(Qt::Key_T);
}

void QFxKeyActions::setKey_T(const QString &s)
{
    d->setKeyExpr(Qt::Key_T, s);
}

QString QFxKeyActions::key_U() const
{
    return d->keyExpr(Qt::Key_U);
}

void QFxKeyActions::setKey_U(const QString &s)
{
    d->setKeyExpr(Qt::Key_U, s);
}

QString QFxKeyActions::key_V() const
{
    return d->keyExpr(Qt::Key_V);
}

void QFxKeyActions::setKey_V(const QString &s)
{
    d->setKeyExpr(Qt::Key_V, s);
}

QString QFxKeyActions::key_W() const
{
    return d->keyExpr(Qt::Key_W);
}

void QFxKeyActions::setKey_W(const QString &s)
{
    d->setKeyExpr(Qt::Key_W, s);
}

QString QFxKeyActions::key_X() const
{
    return d->keyExpr(Qt::Key_X);
}

void QFxKeyActions::setKey_X(const QString &s)
{
    d->setKeyExpr(Qt::Key_X, s);
}

QString QFxKeyActions::key_Y() const
{
    return d->keyExpr(Qt::Key_Y);
}

void QFxKeyActions::setKey_Y(const QString &s)
{
    d->setKeyExpr(Qt::Key_Y, s);
}

QString QFxKeyActions::key_Z() const
{
    return d->keyExpr(Qt::Key_Z);
}

void QFxKeyActions::setKey_Z(const QString &s)
{
    d->setKeyExpr(Qt::Key_Z, s);
}


/*!
    \qmlproperty string KeyActions::leftArrow
    \qmlproperty string KeyActions::rightArrow
    \qmlproperty string KeyActions::upArrow
    \qmlproperty string KeyActions::downArrow

    The action to take for the given arrow key.

    The following example sets actions for the left and right arrow keys.
    \qml
    KeyActions { leftArrow: "print('You pressed left')"; rightArrow: "print('You pressed right')" }
    \endqml
*/

QString QFxKeyActions::key_Left() const
{
    return d->keyExpr(Qt::Key_Left);
}

void QFxKeyActions::setKey_Left(const QString &s)
{
    d->setKeyExpr(Qt::Key_Left, s);
}

QString QFxKeyActions::key_Right() const
{
    return d->keyExpr(Qt::Key_Right);
}

void QFxKeyActions::setKey_Right(const QString &s)
{
    d->setKeyExpr(Qt::Key_Right, s);
}

QString QFxKeyActions::key_Up() const
{
    return d->keyExpr(Qt::Key_Up);
}

void QFxKeyActions::setKey_Up(const QString &s)
{
    d->setKeyExpr(Qt::Key_Up, s);
}

QString QFxKeyActions::key_Down() const
{
    return d->keyExpr(Qt::Key_Down);
}

void QFxKeyActions::setKey_Down(const QString &s)
{
    d->setKeyExpr(Qt::Key_Down, s);
}

/*!
    \qmlproperty string KeyActions::digit0...digit9

    The action to take for the given number key.

    The following example sets actions for the '5' and '6' keys.
    \qml
    KeyActions { digit5: "print('5 is a prime number')"; digit6: "print('6 is a composite number')"; focus: true }
    \endqml
*/

QString QFxKeyActions::key_0() const
{
    return d->keyExpr(Qt::Key_0);
}

void QFxKeyActions::setKey_0(const QString &s)
{
    d->setKeyExpr(Qt::Key_0, s);
}

QString QFxKeyActions::key_1() const
{
    return d->keyExpr(Qt::Key_1);
}

void QFxKeyActions::setKey_1(const QString &s)
{
    d->setKeyExpr(Qt::Key_1, s);
}

QString QFxKeyActions::key_2() const
{
    return d->keyExpr(Qt::Key_2);
}

void QFxKeyActions::setKey_2(const QString &s)
{
    d->setKeyExpr(Qt::Key_2, s);
}

QString QFxKeyActions::key_3() const
{
    return d->keyExpr(Qt::Key_3);
}

void QFxKeyActions::setKey_3(const QString &s)
{
    d->setKeyExpr(Qt::Key_3, s);
}

QString QFxKeyActions::key_4() const
{
    return d->keyExpr(Qt::Key_4);
}

void QFxKeyActions::setKey_4(const QString &s)
{
    d->setKeyExpr(Qt::Key_4, s);
}

QString QFxKeyActions::key_5() const
{
    return d->keyExpr(Qt::Key_5);
}

void QFxKeyActions::setKey_5(const QString &s)
{
    d->setKeyExpr(Qt::Key_5, s);
}

QString QFxKeyActions::key_6() const
{
    return d->keyExpr(Qt::Key_6);
}

void QFxKeyActions::setKey_6(const QString &s)
{
    d->setKeyExpr(Qt::Key_6, s);
}

QString QFxKeyActions::key_7() const
{
    return d->keyExpr(Qt::Key_7);
}

void QFxKeyActions::setKey_7(const QString &s)
{
    d->setKeyExpr(Qt::Key_7, s);
}

QString QFxKeyActions::key_8() const
{
    return d->keyExpr(Qt::Key_8);
}

void QFxKeyActions::setKey_8(const QString &s)
{
    d->setKeyExpr(Qt::Key_8, s);
}

QString QFxKeyActions::key_9() const
{
    return d->keyExpr(Qt::Key_9);
}

void QFxKeyActions::setKey_9(const QString &s)
{
    d->setKeyExpr(Qt::Key_9, s);
}

QString QFxKeyActions::key_Asterisk() const
{
    return d->keyExpr(Qt::Key_Asterisk);
}

void QFxKeyActions::setKey_Asterisk(const QString &s)
{
    d->setKeyExpr(Qt::Key_Asterisk, s);
}

QString QFxKeyActions::key_Escape() const
{
    return d->keyExpr(Qt::Key_Escape);
}

void QFxKeyActions::setKey_Escape(const QString &s)
{
    d->setKeyExpr(Qt::Key_Escape, s);
}

QString QFxKeyActions::key_Return() const
{
    return d->keyExpr(Qt::Key_Return);
}

void QFxKeyActions::setKey_Return(const QString &s)
{
    d->setKeyExpr(Qt::Key_Return, s);
}

QString QFxKeyActions::key_Enter() const
{
    return d->keyExpr(Qt::Key_Enter);
}

void QFxKeyActions::setKey_Enter(const QString &s)
{
    d->setKeyExpr(Qt::Key_Enter, s);
}

QString QFxKeyActions::key_Delete() const
{
    return d->keyExpr(Qt::Key_Delete);
}

void QFxKeyActions::setKey_Delete(const QString &s)
{
    d->setKeyExpr(Qt::Key_Delete, s);
}

QString QFxKeyActions::key_Space() const
{
    return d->keyExpr(Qt::Key_Space);
}

void QFxKeyActions::setKey_Space(const QString &s)
{
    d->setKeyExpr(Qt::Key_Space, s);
}

/*!
    \qmlproperty string KeyActions::escape
    \qmlproperty string KeyActions::keyReturn
    \qmlproperty string KeyActions::enter
    \qmlproperty string KeyActions::delete
    \qmlproperty string KeyActions::space

    The action to take for the given utility key.

    The following example sets an action for the space key.
    \qml
    KeyActions { space: "print('Space pressed')" }
    \endqml
*/

/*!
    \qmlproperty string KeyActions::back
    \qmlproperty string KeyActions::select
    \qmlproperty string KeyActions::yes
    \qmlproperty string KeyActions::no
    \qmlproperty string KeyActions::context1
    \qmlproperty string KeyActions::context2
    \qmlproperty string KeyActions::context3
    \qmlproperty string KeyActions::context4
    \qmlproperty string KeyActions::call
    \qmlproperty string KeyActions::hangup
    \qmlproperty string KeyActions::flip

    The action to take for the given device key.

    The following example sets an action for the hangup key.
    \qml
    KeyActions { hangup: "print('Go away now')" }
    \endqml
*/

QString QFxKeyActions::key_Back() const
{
    return d->keyExpr(Qt::Key_Back);
}

void QFxKeyActions::setKey_Back(const QString &s)
{
    d->setKeyExpr(Qt::Key_Back, s);
}

QString QFxKeyActions::key_Select() const
{
    return d->keyExpr(Qt::Key_Select);
}

void QFxKeyActions::setKey_Select(const QString &s)
{
    d->setKeyExpr(Qt::Key_Select, s);
}

QString QFxKeyActions::key_Yes() const
{
    return d->keyExpr(Qt::Key_Yes);
}

void QFxKeyActions::setKey_Yes(const QString &s)
{
    d->setKeyExpr(Qt::Key_Yes, s);
}

QString QFxKeyActions::key_No() const
{
    return d->keyExpr(Qt::Key_No);
}

void QFxKeyActions::setKey_No(const QString &s)
{
    d->setKeyExpr(Qt::Key_No, s);
}

QString QFxKeyActions::key_Context1() const
{
    return d->keyExpr(Qt::Key_Context1);
}

void QFxKeyActions::setKey_Context1(const QString &s)
{
    d->setKeyExpr(Qt::Key_Context1, s);
}

QString QFxKeyActions::key_Context2() const
{
    return d->keyExpr(Qt::Key_Context2);
}

void QFxKeyActions::setKey_Context2(const QString &s)
{
    d->setKeyExpr(Qt::Key_Context2, s);
}

QString QFxKeyActions::key_Context3() const
{
    return d->keyExpr(Qt::Key_Context3);
}

void QFxKeyActions::setKey_Context3(const QString &s)
{
    d->setKeyExpr(Qt::Key_Context3, s);
}

QString QFxKeyActions::key_Context4() const
{
    return d->keyExpr(Qt::Key_Context4);
}

void QFxKeyActions::setKey_Context4(const QString &s)
{
    d->setKeyExpr(Qt::Key_Context4, s);
}

QString QFxKeyActions::key_Call() const
{
    return d->keyExpr(Qt::Key_Call);
}

void QFxKeyActions::setKey_Call(const QString &s)
{
    d->setKeyExpr(Qt::Key_Call, s);
}

QString QFxKeyActions::key_Hangup() const
{
    return d->keyExpr(Qt::Key_Hangup);
}

void QFxKeyActions::setKey_Hangup(const QString &s)
{
    d->setKeyExpr(Qt::Key_Hangup, s);
}

QString QFxKeyActions::key_Flip() const
{
    return d->keyExpr(Qt::Key_Flip);
}

void QFxKeyActions::setKey_Flip(const QString &s)
{
    d->setKeyExpr(Qt::Key_Flip, s);
}

/*!
    \qmlproperty string KeyActions::any

    The action to take for any key not otherwise handled.
*/
QString QFxKeyActions::key_Any() const
{
    return d->keyExpr(Qt::Key_unknown);
}

void QFxKeyActions::setKey_Any(const QString &s)
{
    d->setKeyExpr(Qt::Key_unknown, s);
}

void QFxKeyActions::keyPressEvent(QKeyEvent *event)
{
    Qt::Key key = (Qt::Key)event->key();
    if (d->enabled && d->key(key)) {
        QmlExpression b(qmlContext(this), d->action(key), this);
        b.setTrackChange(false);
        b.value();
        event->accept();
    } else {
        QFxItem::keyPressEvent(event);
    }
}

void QFxKeyActions::keyReleaseEvent(QKeyEvent *event)
{
    Qt::Key key = (Qt::Key)event->key();
    if (d->enabled && d->key(key)) {
        event->accept();
    } else {
        QFxItem::keyReleaseEvent(event);
    }
}

QT_END_NAMESPACE
